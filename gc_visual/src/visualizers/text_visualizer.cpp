/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/visualizers/text_visualizer.hpp"

#include "gc_visual/graph_broker.hpp"
#include "gc_visual/qstr.hpp"

#include <yaml-cpp/yaml.h>

#include <QBoxLayout>
#include <QTextEdit>


struct TextVisualizer::Storage
{
    QTextEdit* view{};
};

TextVisualizer::TextVisualizer(GraphBroker*,
                               const YAML::Node&,
                               QWidget* parent):
    VisualizerWidget{ parent },
    storage_{std::make_unique<Storage>()}
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* layout = new QVBoxLayout{};
    setLayout(layout);

    auto view = new QTextEdit{};
    storage_->view = view;
    layout->addWidget(view);

    view->setReadOnly(true);
    view->setAcceptRichText(false);
}

TextVisualizer::~TextVisualizer() = default;

auto TextVisualizer::check_type(const gc::Type*) -> TypeCheckResult
{
    return { .ok = true };
}

void TextVisualizer::set_value(const gc::Value& v)
{
    constexpr auto max_lines = 1000ul;

    auto* view = storage_->view;

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
}
