/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/editors/cell2d_rule_editor_widget.hpp"

#include "gc_visual/widgets/cell2d_rule_map_view.hpp"

#include "gc/value.hpp"

#include "common/scoped_inc.hpp"

#include <yaml-cpp/yaml.h>

#include <QBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QScrollArea>
#include <QSpinBox>

auto Cell2dRuleEditorWidget::RuleMapWidgets::setVisible(bool visible) -> void
{
    label->setVisible(visible);
    scroll_view->setVisible(visible);
}

Cell2dRuleEditorWidget::Cell2dRuleEditorWidget(const YAML::Node&,
                                               QWidget* parent) :
    ParameterEditorWidget{ parent }
{
    auto* layout = new QVBoxLayout{};
    setLayout(layout);

    auto* sub_layout = new QHBoxLayout{};
    layout->addLayout(sub_layout);

    auto make_spin = [&](QString label_text, int min, int max)
    {
        auto* spin_label = new QLabel(label_text);
        sub_layout->addWidget(spin_label);
        auto* spin = new QSpinBox{};
        spin->setMinimum(min);
        spin->setMaximum(max);
        sub_layout->addWidget(spin);
        spin_label->setBuddy(spin);
        return spin;
    };

    state_count_ = make_spin("state count", 2, 255);
    min_state_ = make_spin("min. state", -127, 0);

    sub_layout = new QHBoxLayout{};
    layout->addLayout(sub_layout);

    tor_ = new QCheckBox("tor");
    sub_layout->addWidget(tor_);

    count_self_ = new QCheckBox("count self");
    sub_layout->addWidget(count_self_);

    auto make_map = [&](uint8_t max_neighborhood_size) -> RuleMapWidgets
    {
        auto* label =
            new QLabel("map " + QString::number(max_neighborhood_size));
        layout->addWidget(label);
        auto* view = new Cell2dRuleMapView{max_neighborhood_size};
        auto scroll_view = new QScrollArea{};
        scroll_view->setWidget(view);
        layout->addWidget(scroll_view);
        label->setBuddy(scroll_view);
        return { label, view, scroll_view };
    };

    map9_ = make_map(9);
    map6_ = make_map(6);
    map4_ = make_map(4);

    auto connect_map_view =
        [&](Cell2dRuleMapView* view, std::vector<int8_t>& dst)
        {
            connect(view,
                    &Cell2dRuleMapView::map_changed,
                    [&](const std::vector<int8_t>& map) {
                        dst = map;
                        emit value_changed(rules_);
                    });
        };

    connect_map_view(map9_.view, rules_.map9);
    connect_map_view(map6_.view, rules_.map6);
    connect_map_view(map4_.view, rules_.map4);

    connect(state_count_, &QSpinBox::valueChanged,
            [&](int value) {
                if (in_set_value_ > 0)
                    return;
                auto state_count = uint8_t(value);
                rules_.state_count = state_count;
                rules_.map9.resize(9*state_count, 0);
                rules_.map6.resize(6*state_count, 0);
                rules_.map4.resize(4*state_count, 0);
                map9_.view->set_state_count(state_count);
                map6_.view->set_state_count(state_count);
                map4_.view->set_state_count(state_count);
                emit value_changed(rules_);
            });

    connect(min_state_, &QSpinBox::valueChanged,
            [&](int value) {
                if (in_set_value_ > 0)
                    return;
                auto min_state = int8_t(value);
                rules_.min_state = min_state;
                map9_.view->set_min_state(min_state);
                rules_.map9 = map9_.view->map();
                map6_.view->set_min_state(min_state);
                rules_.map6 = map6_.view->map();
                map4_.view->set_min_state(min_state);
                rules_.map4 = map4_.view->map();
                emit value_changed(rules_);
            });

    connect(tor_, &QCheckBox::clicked,
            [&](bool checked) {
                if (in_set_value_ > 0)
                    return;
                rules_.tor = checked;
                map6_.setVisible(!checked);
                map4_.setVisible(!checked);
                emit value_changed(rules_);
            });

    connect(count_self_, &QCheckBox::clicked,
            [&](bool checked) {
                if (in_set_value_ > 0)
                    return;
                rules_.count_self = checked;
                emit value_changed(rules_);
            });
}

auto Cell2dRuleEditorWidget::value() const
    -> gc::Value
{ return rules_; }

auto Cell2dRuleEditorWidget::check_type(const gc::Type* type) -> TypeCheckResult
{
    static auto expected_type = gc::type_of<gc_app::Cell2dRules>();

    if (type == expected_type)
        return { .ok = true };

    return {
        .ok = false,
        .expected_type_description = common::format(expected_type)
    };
}

void Cell2dRuleEditorWidget::set_value(const gc::Value& v)
{
    auto inc_in_set_value = common::ScopedInc{in_set_value_};

    rules_ = v.as<gc_app::Cell2dRules>();

    state_count_->setValue(rules_.state_count);
    min_state_->setValue(rules_.min_state);
    tor_->setChecked(rules_.tor);
    count_self_->setChecked(rules_.count_self);

    map9_.view->set_state_count(rules_.state_count);
    map9_.view->set_min_state(rules_.min_state);
    map9_.view->set_map(rules_.map9);

    map6_.view->set_state_count(rules_.state_count);
    map6_.view->set_min_state(rules_.min_state);
    map6_.view->set_map(rules_.map6);
    map6_.setVisible(!rules_.tor);

    map4_.view->set_state_count(rules_.state_count);
    map4_.view->set_min_state(rules_.min_state);
    map4_.view->set_map(rules_.map4);
    map4_.setVisible(!rules_.tor);
}
