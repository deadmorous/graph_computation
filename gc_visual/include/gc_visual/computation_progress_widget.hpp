/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/node_index.hpp"

#include <QWidget>

class QLabel;
class QProgressBar;

class ComputationProgressWidget :
    public QWidget
{
    Q_OBJECT

public:
    explicit ComputationProgressWidget(QWidget* parent = nullptr);

signals:
    auto stop()
        -> void;

public slots:
    auto set_progress(gc::NodeIndex inode, double node_progress)
        -> void;

private:
    QLabel* label_;
    QProgressBar* progress_bar_;
};
