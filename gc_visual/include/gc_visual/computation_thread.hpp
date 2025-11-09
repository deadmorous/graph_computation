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

#include "gc_visual/graph_evolution.hpp"

#include "gc/graph_computation.hpp"
#include "gc/param_spec.hpp"

#include <QThread>


class ComputationThread :
    public QThread
{
    Q_OBJECT
public:
    explicit ComputationThread(QObject* parent = nullptr);

    auto computation()
        -> gc::Computation&;

    auto get_parameter(const gc::ParameterSpec&) const
        -> gc::Value;

    auto ok()
        -> bool;

    auto evolution() const
        -> std::optional<gc_visual::GraphEvolution>;

signals:
    auto progress(gc::NodeIndex inode, double node_progress)
        -> void;

    auto running_state_changed(bool running)
        -> void;

public slots:
    auto advance_evolution(size_t skip = 0)
        -> void;

    auto reset_evolution()
        -> void;

    auto set_evolution(std::optional<gc_visual::GraphEvolution>)
        -> void;

    auto set_graph(gc::ComputationGraph g,
                   const gc::SourceInputs& provided_inputs)
        -> void;

    auto set_parameter(const gc::ParameterSpec&, const gc::Value&)
        -> void;

    auto stop()
        -> void;

private slots:
    auto on_started() -> void;
    auto on_finished() -> void;

protected:
    auto run() -> void override;

private:
    auto clear_feedback() -> void;
    auto set_feedback() -> void;

    bool ok_;
    std::stop_source stop_source_;
    gc::Computation computation_;
    std::optional<gc_visual::GraphEvolution> evolution_;

    // Zero value is used in a non-evolution mode, and a positive value -
    // in the feedback-driven evolution mode
    size_t skip_ = 0;
};
