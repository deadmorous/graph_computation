#pragma once

#include "gc/graph_computation.hpp"

#include <QThread>


class ComputationThread :
    public QThread
{
    Q_OBJECT
public:
    explicit ComputationThread(QObject* parent = nullptr);

    auto computation()
        -> gc::Computation&;

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

    auto set_graph(gc::Graph g)
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
