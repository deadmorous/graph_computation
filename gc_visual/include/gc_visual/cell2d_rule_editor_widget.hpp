/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_app/types/cell2d_rules.hpp"

#include "gc/value.hpp"

#include <QWidget>

class Cell2dRuleMapView;

class QCheckBox;
class QLabel;
class QScrollArea;
class QSpinBox;

class Cell2dRuleEditorWidget final :
    public QWidget
{
    Q_OBJECT
public:
    Cell2dRuleEditorWidget(gc::Value v, QWidget* parent = nullptr);

    auto value() const noexcept
        -> gc::Value;

signals:
    auto valueChanged(const gc::Value& v) -> void;

public slots:
    auto setValue(gc::Value v) -> void;

protected:

private:
    struct RuleMapWidgets final
    {
        QLabel* label{};
        Cell2dRuleMapView* view{};
        QScrollArea* scroll_view{};

        auto setVisible(bool visible) -> void;
    };

    gc_app::Cell2dRules rules_;

    QSpinBox* state_count_{};
    QSpinBox* min_state_{};
    QCheckBox* tor_{};
    QCheckBox* count_self_{};

    RuleMapWidgets map9_{};
    RuleMapWidgets map6_{};
    RuleMapWidgets map4_{};
};
