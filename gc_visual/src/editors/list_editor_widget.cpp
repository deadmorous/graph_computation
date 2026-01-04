/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/editors/list_editor_widget.hpp"

#include "gc/value.hpp"

#include "common/scoped_inc.hpp"


using namespace std::string_view_literals;

ListEditorWidget::ListEditorWidget(const YAML::Node&, QWidget* parent) :
    ParameterEditorWidgetWrapper<QComboBox>{ new QComboBox{}, parent }
{
    connect(widget_, &QComboBox::currentIndexChanged, this,
            [&]{
                if (in_set_value_ > 0)
                    return;
                emit value_changed(value());
            });
}

auto ListEditorWidget::value() const -> gc::Value
{
    if (!type_)
        return {};

    auto result = gc::Value::make(type_);
    auto index = widget_->currentIndex();
    if (index < 0)
        return result;

    result.set(gc::ValuePath{"index"sv}, size_t(index));
    return result;
}

auto ListEditorWidget::check_type(const gc::Type* type) -> TypeCheckResult
{
    if (type->aggregate_type() == gc::AggregateType::Enum)
        return { .ok = true };

    return {
        .ok = false,
        .expected_type_description = "an enumerated type"
    };
}

void ListEditorWidget::set_value(const gc::Value& value)
{
    auto inc_in_set_value = common::ScopedInc{in_set_value_};

    auto type = value.type();
    if (type_ != type)
    {
        widget_->clear();
        auto names =
            value.get(gc::ValuePath{"names"sv})
                .as<std::vector<std::string_view>>();

        for (auto name : names)
            widget_->addItem(QString::fromUtf8(name));

        type_ = type;
    }

    auto index = value.get(gc::ValuePath{"index"sv}).convert_to<int>();
    widget_->setCurrentIndex(index);
}
