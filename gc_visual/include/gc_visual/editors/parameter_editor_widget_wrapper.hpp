/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc_visual/editors/parameter_editor_widget.hpp"

#include <QBoxLayout>


template <typename Widget>
class ParameterEditorWidgetWrapper : public ParameterEditorWidget
{
public:
    explicit ParameterEditorWidgetWrapper(
        Widget* widget, QWidget* parent = nullptr) :
        ParameterEditorWidget{parent},
        widget_{widget}
    {
        auto* layout = new QHBoxLayout{};
        setLayout(layout);
        layout->addWidget(widget_);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        setSizePolicy(widget_->sizePolicy());
    }

    QSize sizeHint() const override
    { return widget_->sizeHint(); }

    QSize minimumSizeHint() const override
    { return widget_->minimumSizeHint(); }

    bool hasHeightForWidth() const override
    { return widget_->hasHeightForWidth(); }

    int heightForWidth(int w) const override
    { return widget_->heightForWidth(w); }

protected:
    Widget* widget_;
};
