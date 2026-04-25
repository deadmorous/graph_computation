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

#include "mpk/mix/value/value.hpp"

#include "mpk/mix/util/scoped_inc.hpp"


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

auto ListEditorWidget::value() const -> mpk::mix::value::Value
{
    if (!type_)
        return {};

    auto result = mpk::mix::value::Value::make(type_);
    auto index = widget_->currentIndex();
    if (index < 0)
        return result;

    result.set(mpk::mix::value::ValuePath{"index"sv}, size_t(index));
    return result;
}

auto ListEditorWidget::check_type(const mpk::mix::value::Type* type) -> TypeCheckResult
{
    if (type->aggregate_type() == mpk::mix::value::AggregateType::Enum)
        return { .ok = true };

    return {
        .ok = false,
        .expected_type_description = "an enumerated type"
    };
}

void ListEditorWidget::set_value(const mpk::mix::value::Value& value)
{
    auto inc_in_set_value = mpk::mix::ScopedInc{in_set_value_};

    auto type = value.type();
    if (type_ != type)
    {
        widget_->clear();
        auto names =
            value.get(mpk::mix::value::ValuePath{"names"sv})
                .as<std::vector<std::string_view>>();

        for (auto name : names)
            widget_->addItem(QString::fromUtf8(name));

        type_ = type;
    }

    auto index = value.get(mpk::mix::value::ValuePath{"index"sv}).convert_to<int>();
    widget_->setCurrentIndex(index);
}
