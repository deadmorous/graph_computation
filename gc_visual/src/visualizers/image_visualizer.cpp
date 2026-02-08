/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/visualizers/image_visualizer.hpp"

#include "gc_visual/graph_broker.hpp"
#include "gc_visual/video_recorder.hpp"
#include "gc_visual/widgets/bitmap_view.hpp"

#include "gc_types/image.hpp"

#include <yaml-cpp/yaml.h>

#include <QBoxLayout>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>

#include <magic_enum/magic_enum.hpp>


struct ImageVisualizer::Storage
{
    BitmapView* bitmap_view{};
};

ImageVisualizer::ImageVisualizer(GraphBroker* broker,
                                 const YAML::Node& item_node,
                                 QWidget* parent):
    VisualizerWidget{ parent },
    storage_{std::make_unique<Storage>()}
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* layout = new QVBoxLayout{};
    setLayout(layout);

    auto* sub_layout = new QHBoxLayout{};
    layout->addLayout(sub_layout);

    auto* scale_slider = new QSlider{ Qt::Horizontal };
    scale_slider->setMinimum(1);
    scale_slider->setMaximum(100);
    sub_layout->addWidget(scale_slider);

    QDoubleSpinBox* blend_spin{};

    auto blend_factor = std::optional<double>{};
    auto blend_mode = BitmapView::BlendMode::none;
    if (auto blend_factor_node = item_node["blend_factor"];
        blend_factor_node.IsDefined())
    {
        auto* blend_spin_label = new QLabel("blend");
        blend_spin = new QDoubleSpinBox{};
        blend_spin_label->setBuddy(blend_spin);
        blend_spin->setRange(0, 1);
        blend_spin->setDecimals(2);
        blend_spin->setSingleStep(0.1);
        sub_layout->addWidget(blend_spin_label);
        sub_layout->addWidget(blend_spin);

        blend_factor = blend_factor_node.as<double>();
        blend_spin->setValue(*blend_factor);

        auto blend_mode_node = item_node["blend_mode"];
        if (blend_mode_node.IsDefined())
        {
            auto mode_str = blend_mode_node.as<std::string>();
            auto opt_blend_mode =
                magic_enum::enum_cast<BitmapView::BlendMode>(mode_str);
            if (!opt_blend_mode)
                common::throw_(
                    "Invalid blend mode '", mode_str, "', expected one of ",
                    common::format_seq(
                        magic_enum::enum_names<BitmapView::BlendMode>(), ", "));
            blend_mode = *opt_blend_mode;
        }
    }

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

    auto* bitmap_view = new BitmapView{blend_mode, blend_factor.value_or(0.)};
    storage_->bitmap_view = bitmap_view;
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
        scale_slider, &QSlider::valueChanged,
        bitmap_view,
        [=](int pos) { bitmap_view->set_scale(1. + (pos-1)/10.); });

    if (blend_factor)
        QObject::connect(
            blend_spin, &QDoubleSpinBox::valueChanged,
            bitmap_view, &BitmapView::set_blend_factor);

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

    QObject::connect(
        bitmap_view, &BitmapView::image_updated,
        video_recorder, &VideoRecorder::enqueue_frame);

    QObject::connect(
        video_recorder, &VideoRecorder::error,
        broker, &GraphBroker::gui_error);

}

ImageVisualizer::~ImageVisualizer() = default;

auto ImageVisualizer::check_type(const gc::Type* type) -> TypeCheckResult
{
    static auto expected_type = gc::type_of<gc_types::ColorImage>();

    if (type == expected_type)
        return { .ok = true };

    return {
        .ok = false,
        .expected_type_description = common::format(expected_type)
    };
}

void ImageVisualizer::set_value(const gc::Value& v)
{
    storage_->bitmap_view->set_image(v);
}
