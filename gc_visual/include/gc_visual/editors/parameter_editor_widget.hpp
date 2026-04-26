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

#include "gc_visual/type_check_result.hpp"

#include "mpk/mix/value/type_fwd.hpp"
#include "mpk/mix/value/value_fwd.hpp"
#include "mpk/mix/value/value_path.hpp"

#include <yaml-cpp/node/node.h>

#include <QWidget>


class ParameterEditorWidget :
    public QWidget
{
    Q_OBJECT

public:
    explicit ParameterEditorWidget(QWidget* parent = nullptr);

    virtual ~ParameterEditorWidget() = default;

    virtual auto value() const -> mpk::mix::value::Value = 0;

signals:
    void value_changed(const mpk::mix::value::Value&, mpk::mix::value::ValuePathView path={});

public slots:
    virtual void set_value(const mpk::mix::value::Value&) = 0;
};
