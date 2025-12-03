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

#include "gc_visual/parameter_editor_widget.hpp"

#include <QWidget>

class Cell2dRuleMapView;

class QCheckBox;
class QLabel;
class QScrollArea;
class QSpinBox;

class Cell2dRuleEditorWidget final :
    public ParameterEditorWidget
{
    Q_OBJECT

public:
    explicit Cell2dRuleEditorWidget(const YAML::Node&,
                                    QWidget* parent = nullptr);

    auto value() const -> gc::Value override;

    static auto check_type(const gc::Type*) -> TypeCheckResult;

public slots:
    void set_value(const gc::Value& v) override;

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

    int in_set_value_{};
};
