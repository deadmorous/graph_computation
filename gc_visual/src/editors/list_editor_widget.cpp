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

#include <QBoxLayout>
#include <QComboBox>


using namespace std::string_view_literals;

ListEditorWidget::ListEditorWidget(const YAML::Node&, QWidget* parent) :
    ParameterEditorWidget{ parent }
{
    auto* layout = new QHBoxLayout{};
    setLayout(layout);

    combo_ = new QComboBox{};
    layout->addWidget(combo_);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setSizePolicy(combo_->sizePolicy());

    connect(combo_, &QComboBox::currentIndexChanged, this,
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
    auto index = combo_->currentIndex();
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

QSize ListEditorWidget::sizeHint() const
{
    return combo_->sizeHint();
}

QSize ListEditorWidget::minimumSizeHint() const
{
    return combo_->minimumSizeHint();
}

bool ListEditorWidget::hasHeightForWidth() const
{
    return combo_->hasHeightForWidth();
}

int ListEditorWidget::heightForWidth(int w) const
{
    return combo_->heightForWidth(w);
}

void ListEditorWidget::set_value(const gc::Value& value)
{
    auto inc_in_set_value = common::ScopedInc{in_set_value_};

    auto type = value.type();
    if (type_ != type)
    {
        combo_->clear();
        auto names =
            value.get(gc::ValuePath{"names"sv})
                .as<std::vector<std::string_view>>();

        for (auto name : names)
            combo_->addItem(QString::fromUtf8(name));

        type_ = type;
    }

    auto index = value.get(gc::ValuePath{"index"sv}).convert_to<int>();
    combo_->setCurrentIndex(index);

    emit value_changed(this->value());
}
