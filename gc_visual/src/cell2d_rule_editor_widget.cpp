/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/cell2d_rule_editor_widget.hpp"

#include "gc_visual/cell2d_rule_map_view.hpp"

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

Cell2dRuleEditorWidget::Cell2dRuleEditorWidget(gc::Value v, QWidget* parent) :
    QWidget{ parent }
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
                        emit valueChanged(rules_);
                    });
        };

    connect_map_view(map9_.view, rules_.map9);
    connect_map_view(map6_.view, rules_.map6);
    connect_map_view(map4_.view, rules_.map4);

    connect(state_count_, &QSpinBox::valueChanged,
            [&](int value) {
                auto state_count = uint8_t(value);
                rules_.state_count = state_count;
                map9_.view->set_state_count(state_count);
                map6_.view->set_state_count(state_count);
                map4_.view->set_state_count(state_count);
                emit valueChanged(rules_);
            });

    connect(min_state_, &QSpinBox::valueChanged,
            [&](int value) {
                auto min_state = int8_t(value);
                rules_.min_state = min_state;
                map9_.view->set_min_state(min_state);
                rules_.map9 = map9_.view->map();
                map6_.view->set_min_state(min_state);
                rules_.map6 = map6_.view->map();
                map4_.view->set_min_state(min_state);
                rules_.map4 = map4_.view->map();
                emit valueChanged(rules_);
            });

    connect(tor_, &QCheckBox::clicked,
            [&](bool checked) {
                rules_.tor = checked;
                map6_.setVisible(!checked);
                map4_.setVisible(!checked);
                emit valueChanged(rules_);
            });

    connect(count_self_, &QCheckBox::clicked,
            [&](bool checked) {
                rules_.count_self = checked;
                emit valueChanged(rules_);
            });

    setValue(v);
}

auto Cell2dRuleEditorWidget::value() const noexcept
    -> gc::Value
{ return rules_; }

auto Cell2dRuleEditorWidget::setValue(gc::Value v) -> void
{
    rules_ = v.as<gc_app::Cell2dRules>();

    state_count_->setValue(rules_.state_count);
    min_state_->setValue(rules_.min_state);
    tor_->setChecked(rules_.tor);
    count_self_->setChecked(rules_.count_self);

    map9_.view->set_map(rules_.map9);
    map6_.view->set_map(rules_.map6);
    map6_.setVisible(!rules_.tor);
    map4_.view->set_map(rules_.map4);
    map4_.setVisible(!rules_.tor);

}
