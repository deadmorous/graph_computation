#include "gc_visual/computation_thread.hpp"

#include "common/func_ref.hpp"

#include <QtGlobal>

ComputationThread::ComputationThread(QObject* parent) :
    QThread{ parent },
    ok_{ false }
{
    connect(this, &ComputationThread::started,
            this, &ComputationThread::on_started);
    connect(this, &ComputationThread::finished,
            this, &ComputationThread::on_finished);
}

auto ComputationThread::computation()
    -> gc::Computation&
{
    Q_ASSERT(!isRunning());
    return computation_;
}

auto ComputationThread::ok()
    -> bool
{ return ok_; }

auto ComputationThread::stop()
    -> void
{
    if (!isRunning())
        return;

    stop_source_.request_stop();
    wait();
}

auto ComputationThread::set_graph(gc::Graph g)
    -> void
{
    stop();
    computation_ = gc::computation(std::move(g));
}

auto ComputationThread::on_started()
    -> void
{ emit running_state_changed(true); }

auto ComputationThread::on_finished()
    -> void
{ emit running_state_changed(false); }

auto ComputationThread::run()
    -> void
{
    ok_ = false;

    stop_source_ = {};

    auto graph_progress =
        [this](uint32_t inode, double node_progress)
    { emit progress(inode, node_progress); };

    ok_ = compute(computation_, stop_source_.get_token(), &graph_progress);
}
