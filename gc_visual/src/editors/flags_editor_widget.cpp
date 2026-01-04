/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/editors/flags_editor_widget.hpp"

#include "gc/value.hpp"

#include "common/format.hpp"
#include "common/scoped_inc.hpp"

#include <QMenu>

#include <ranges>


using namespace std::string_view_literals;

namespace {

auto make_key(const gc::Type* key_type, std::string_view name) -> gc::Value
{
    auto k = gc::Value::make(key_type);
    k.set(gc::ValuePath{"name"sv}, std::string(name));
    return k;
}

auto make_key(const gc::Type* key_type, size_t index) -> gc::Value
{
    auto k = gc::Value::make(key_type);
    k.set(gc::ValuePath{"index"sv}, index);
    return k;
}

auto flag_names(const gc::Type* key_type) -> std::vector<std::string_view>
{
    auto k = gc::Value::make(key_type);
    return k.get(gc::ValuePath{"names"sv})
        .as<std::vector<std::string_view>>();
}

auto key_type(const gc::Type* type) -> const gc::Type*
{
    return gc::SetT{type}.key_type();
}

} // anonymous namespace

FlagsEditorWidget::FlagsEditorWidget(const YAML::Node&, QWidget* parent) :
    ParameterEditorWidgetWrapper<QToolButton>{ new QToolButton{}, parent }
{
    widget_->setText("flags");
    widget_->setPopupMode(QToolButton::InstantPopup);
    auto* menu = new QMenu(widget_);
    widget_->setMenu(menu);
}

auto FlagsEditorWidget::value() const -> gc::Value
{
    return value_;
}

auto FlagsEditorWidget::check_type(const gc::Type* type) -> TypeCheckResult
{
    if (type->aggregate_type() == gc::AggregateType::Set)
    {
        const auto* key_type = gc::SetT{type}.key_type();
        if (key_type->aggregate_type() == gc::AggregateType::Enum)
            return { .ok = true };
    }

    return {
        .ok = false,
        .expected_type_description = "a set of flags of enumerated type items"
    };
}

void FlagsEditorWidget::set_value(const gc::Value& value)
{
    if (value_ == value)
        return;

    auto inc_in_set_value = common::ScopedInc{in_set_value_};

    auto* menu = widget_->menu();

    const auto* type = value.type();
    const auto* k_type = key_type(type);
    if (value_.type() != type)
    {
        menu->clear();

        auto names = [&]{
            auto k = gc::Value::make(k_type);
            return k.get(gc::ValuePath{"names"sv})
                .as<std::vector<std::string_view>>();
        }();

        for (auto name : names)
        {
            auto* action = menu->addAction(QString::fromUtf8(name));
            action->setCheckable(true);
            connect(
                action,
                &QAction::toggled,
                this,
                [name, k_type, this](bool checked) {
                    auto k = make_key(k_type, name);
                    if (checked)
                        value_.insert(k);
                    else
                        value_.remove(k);
                    if (in_set_value_ == 0)
                        emit value_changed(value_);
                    update_summary();
                });
        }
    }
    value_ = value;

    for (size_t index = 0; auto* action : menu->actions())
    {
        auto k = make_key(k_type, index++);
        action->setChecked(value_.contains(k));
    }
    update_summary();
}

auto FlagsEditorWidget::update_summary() -> void
{
    widget_->setText(QString::fromUtf8(summary()));
}

auto FlagsEditorWidget::summary() const -> std::string
{
    const auto* type = value_.type();
    if (!type)
        return "flags";

    const auto* k_type = key_type(type);
    auto names = flag_names(k_type);
    auto names_set = names | std::views::filter([&](std::string_view name) {
        return value_.contains(make_key(k_type, name));
    });
    auto size = std::distance(names_set.begin(), names_set.end());
    if (size == 0)
        return "<none>";
    if (size == 1)
        return std::string{names_set.front()};
    if (size == names.size())
        return "<all>";
    return common::format(size, " flags");
}
