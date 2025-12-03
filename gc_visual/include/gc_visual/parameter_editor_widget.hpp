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

#include "gc/type_fwd.hpp"
#include "gc/value_fwd.hpp"
#include "gc/value_path.hpp"

#include <yaml-cpp/node/node.h>

#include <QWidget>


class ParameterEditorWidget :
    public QWidget
{
    Q_OBJECT

public:
    struct TypeCheckResult final
    {
        bool ok;
        std::string expected_type_description;
    };

    explicit ParameterEditorWidget(QWidget* parent = nullptr);

    virtual ~ParameterEditorWidget() = default;

    virtual auto value() const -> gc::Value = 0;

signals:
    void value_changed(const gc::Value&, gc::ValuePathView path={});

public slots:
    virtual void set_value(const gc::Value&) = 0;
};
