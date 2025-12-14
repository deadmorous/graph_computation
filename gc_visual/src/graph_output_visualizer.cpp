/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/graph_output_visualizer.hpp"

#include "gc_visual/bitmap_view.hpp"
#include "gc_visual/graph_broker.hpp"
#include "gc_visual/qstr.hpp"
#include "gc_visual/video_recorder.hpp"

#include "gc/detail/parse_node_port.hpp"

#include <yaml-cpp/yaml.h>

#include <QBoxLayout>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QTextEdit>


using namespace std::string_view_literals;

namespace {

auto make_image(GraphBroker* broker,
                const YAML::Node& item_node,
                GraphOutputVisualizer* parent)
    -> void
{
    parent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* layout = new QVBoxLayout{};
    parent->setLayout(layout);

    // Resolve output port binding
    auto node_port_str = item_node["bind"].as<std::string>();
    auto output_port =
        gc::detail::parse_node_port(node_port_str,
                                    broker->named_nodes(),
                                    broker->node_indices(),
                                    gc::Output);

    auto* sub_layout = new QHBoxLayout{};
    layout->addLayout(sub_layout);

    auto* slider = new QSlider{ Qt::Horizontal };
    slider->setMinimum(1);
    slider->setMaximum(100);
    sub_layout->addWidget(slider);

    auto* record_video_check = new QCheckBox("&Video");
    sub_layout->addWidget(record_video_check);

    auto video_quality = new QSpinBox{parent};
    constexpr int default_video_quality = 23;
    video_quality->setMinimum(0);
    video_quality->setMaximum(51);
    video_quality->setValue(default_video_quality);
    sub_layout->addWidget(video_quality);

    auto* save_button = new QPushButton("Sav&e...");
    sub_layout->addWidget(save_button);

    auto* scroll_view = new QScrollArea{};

    auto* bitmap_view = new BitmapView{};
    scroll_view->setWidget(bitmap_view);

    layout->addWidget(scroll_view);

    auto* video_recorder = new VideoRecorder{parent};

    auto start_video_recording = [=, last_saved_video_name=QString{}]() mutable
    {
        if (video_recorder->status() != VideoRecorderStatus::Ready)
        {
            QMessageBox::critical(
                parent, {}, "Video recording is already in progress");
            return false;
        }

        const auto& image = bitmap_view->qimage();
        if (image.isNull())
        {
            QMessageBox::critical(parent, {}, "Current image is empty");
            return false;
        }


        auto file_name = QFileDialog::getSaveFileName(
            parent, "Save image", last_saved_video_name, "Videos (*.mp4)");

        if (file_name.isEmpty())
            return false;

        auto errors = QStringList{};
        auto on_start_recording_error = [&](const QString& error)
        {
            errors.append(error);
        };

        auto conn_err = QObject::connect(
            video_recorder, &VideoRecorder::error, on_start_recording_error);

        constexpr int default_fps = 25; // TODO: Configure it?
        video_quality->value();

        auto ok = video_recorder->start(
            file_name, image.width(), image.height(),
            {
                .fps = default_fps,
                .h264_quality = video_quality->value()
            });

        QObject::disconnect(conn_err);

        if (!ok)
        {
            QMessageBox::critical(
                parent, {}, "Failed to start recording:\n" + errors.join('\n'));
        }

        last_saved_video_name = file_name;

        return true;
    };

    auto stop_video_recording = [=]
    {
        switch (video_recorder->status())
        {
        case VideoRecorderStatus::Ready:
            QMessageBox::critical(
                parent, {}, "Video recording is not currently in progress");
            return false;
        case VideoRecorderStatus::Recording:
            video_recorder->request_stop();
            return true;
        case VideoRecorderStatus::Finishing:
            QMessageBox::critical(
                parent, {}, "Video recording is currently being finalized");
            return false;
        }
        __builtin_unreachable();
    };

    QObject::connect(
        slider, &QSlider::valueChanged,
        bitmap_view,
        [=](int pos) { bitmap_view->set_scale(1. + (pos-1)/10.); });

    QObject::connect(
        record_video_check, &QCheckBox::clicked,
        [=](bool checked) mutable
        {
            if (checked)
            {
                if (!start_video_recording())
                    record_video_check->setChecked(false);
            }
            else
            {
                if (!stop_video_recording())
                    record_video_check->setChecked(true);
            }
        });

    QObject::connect(
        save_button, &QPushButton::clicked,
        [=, last_saved_name=QString{}]() mutable {
            auto image = bitmap_view->qimage();
            if (image.isNull())
            {
                QMessageBox::critical(parent, {}, "Current image is empty");
                return;
            }

            auto file_name = QFileDialog::getSaveFileName(
                parent, "Save image", last_saved_name, "Images (*.png)");
            if (file_name.isEmpty())
                return;
            if (image.save(file_name))
                last_saved_name = file_name;
            else
                QMessageBox::critical(
                    parent, {}, "Failed to save file " + file_name);
        });

    auto on_output_updated = [=](gc::EdgeOutputEnd output)
    {
        if (output != output_port)
            return;

        bitmap_view->set_image(broker->get_port_value(output_port));
    };

    QObject::connect(
        broker, &GraphBroker::output_updated,
        bitmap_view, on_output_updated);

    on_output_updated(output_port);

    QObject::connect(
        bitmap_view, &BitmapView::image_updated,
        video_recorder, &VideoRecorder::enqueue_frame);

    QObject::connect(
        video_recorder, &VideoRecorder::error,
        broker, &GraphBroker::gui_error);
}

auto make_text(GraphBroker* broker,
                const YAML::Node& item_node,
                GraphOutputVisualizer* parent)
    -> void
{
    parent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* layout = new QVBoxLayout{};
    parent->setLayout(layout);

    // Resolve output port binding
    auto node_port_str = item_node["bind"].as<std::string>();
    auto output_port =
        gc::detail::parse_node_port(node_port_str,
                                    broker->named_nodes(),
                                    broker->node_indices(),
                                    gc::Output);

    auto view = new QTextEdit{};
    layout->addWidget(view);

    view->setReadOnly(true);
    view->setAcceptRichText(false);

    auto on_output_updated = [=](gc::EdgeOutputEnd output)
    {
        if (output != output_port)
            return;

        const auto& v = broker->get_port_value(output_port);

        constexpr auto max_lines = 1000ul;

        if (v.type()->aggregate_type() == gc::AggregateType::Vector)
        {
            view->clear();
            for (size_t i=0, n=std::min(v.size(), max_lines); i<n; ++i)
                view->append(format_qstr(i, '\t', v.get(gc::ValuePath{i})));

            if (v.size() > max_lines)
                view->append("...");
        }
        else
            view->setText(format_qstr(v));

        auto cursor = view->textCursor();
        cursor.setPosition(0);
        view->setTextCursor(cursor);
    };

    QObject::connect(
        broker, &GraphBroker::output_updated,
        view, on_output_updated );

    on_output_updated(output_port);
}

using GraphOutputVisualizerFactoryFunc =
    void(*)(GraphBroker*, const YAML::Node&, GraphOutputVisualizer*);

using GraphOutputVisualizerFactoryMap =
    std::unordered_map<std::string_view, GraphOutputVisualizerFactoryFunc>;



auto editor_factory_map() -> const GraphOutputVisualizerFactoryMap&
{
    static auto result = GraphOutputVisualizerFactoryMap{
        { "image"sv, make_image },
        { "text"sv, make_text },
    };

    return result;
}

} // anonymous namespace


GraphOutputVisualizer::GraphOutputVisualizer(const std::string& type,
                                             GraphBroker* broker,
                                             const YAML::Node& item_node,
                                             QWidget* parent) :
    QWidget{ parent }
{
    editor_factory_map().at(type)(broker, item_node, this);
}

auto GraphOutputVisualizer::supports_type(const std::string& type) -> bool
{
    return editor_factory_map().contains(type);
}
