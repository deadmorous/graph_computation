#pragma once

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

signals:
    auto progress(uint32_t inode, double node_progress)
        -> void;

    auto running_state_changed(bool running)
        -> void;

public slots:
    auto stop()
        -> void;

    auto set_graph(gc::Graph g, const gc::SourceInputs& provided_inputs)
        -> void;

    auto set_parameter(const gc::ParameterSpec&, const gc::Value&)
        -> void;

private slots:
    auto on_started() -> void;
    auto on_finished() -> void;

protected:
    auto run() -> void override;

private:
    bool ok_;
    std::stop_source stop_source_;
    gc::Computation computation_;
};
