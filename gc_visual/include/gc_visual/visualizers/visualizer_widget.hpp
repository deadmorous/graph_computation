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

#include "gc/type_fwd.hpp"
#include "gc/value_fwd.hpp"
#include "gc/value_path.hpp"

#include <yaml-cpp/node/node.h>

#include <QWidget>


class VisualizerWidget :
    public QWidget
{
    Q_OBJECT

public:
    explicit VisualizerWidget(QWidget* parent = nullptr);

    virtual ~VisualizerWidget() = default;

public slots:
    virtual void set_value(const gc::Value&) = 0;
};
