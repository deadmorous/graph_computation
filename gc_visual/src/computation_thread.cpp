/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/computation_thread.hpp"

#include "common/func_ref.hpp"
#include "common/overloads.hpp"

#include <QtGlobal>

ComputationThread::ComputationThread(QObject* parent) :
    QThread{ parent },
    ok_{ false }
{
    connect(this, &ComputationThread::started,
            this, &ComputationThread::on_started);
    connect(this, &ComputationThread::finished,
            this, &ComputationThread::on_finished);
    connect(this, &ComputationThread::queued_start,
            this, &QThread::start, Qt::QueuedConnection);
}

auto ComputationThread::computation()
    -> gc::Computation&
{ return computation_; }

auto ComputationThread::get_parameter(const gc::ParameterSpec& spec) const
    -> gc::Value
{
    return visit(
        common::Overloads{
            [&](const gc::ExternalInputSpec& i) -> gc::Value
            {
                return computation_
                    .source_inputs.values[i.input].get(spec.path);
            },
                [&](const gc::NodeOutputSpec& o) -> gc::Value
            {
                const auto& res = computation_.result;
                auto node_outputs = group(res.outputs, o.output.node);
                assert(node_outputs.index_range().contains(o.output.port));
                return node_outputs[o.output.port].get(spec.path);
            }
        },
        spec.io);
}

auto ComputationThread::ok()
    -> bool
{ return ok_; }

auto ComputationThread::evolution() const
    -> std::optional<gc_visual::GraphEvolution>
{ return evolution_; }

auto ComputationThread::reset_computation()
    -> void
{
    stop();
    skip_ = 0;
    auto& res = computation_.result;
    res.computation_ts = gc::Timestamp{};
    std::ranges::fill(res.node_ts, gc::Timestamp{});
    start_computation();
}

auto ComputationThread::advance_evolution(size_t skip)
    -> void
{
    if (!evolution_)
        return;

    if (isRunning())
        return;

    skip_ = skip;
    start_computation();
}

auto ComputationThread::set_evolution(
                std::optional<gc_visual::GraphEvolution> evolution)
    -> void
{
    stop();
    evolution_ = evolution;
    skip_ = 0;
}

auto ComputationThread::set_graph(gc::ComputationGraph g,
                                  const gc::SourceInputs& provided_inputs)
    -> void
{
    stop();
    computation_ = gc::computation(std::move(g), provided_inputs);
}

auto ComputationThread::set_parameter(const gc::ParameterSpec& spec,
                                      const gc::Value& value)
    -> void
{
    stop();
    skip_ = 0;

    visit(
        common::Overloads{
            [&](const gc::ExternalInputSpec& i)
            {
                computation_.source_inputs
                    .values[i.input].set(spec.path, value);
            },
            [&](const gc::NodeOutputSpec& o)
            {
                auto& res = computation_.result;

                auto node_outputs = group(res.outputs, o.output.node);
                assert(node_outputs.index_range().contains(o.output.port));
                node_outputs[o.output.port].set(spec.path, value);

                for (const auto& e : computation_.graph.edges)
                {
                    if (e.from != o.output)
                        continue;

                    auto node_inputs = group(res.inputs, e.to.node);
                    assert(node_inputs.index_range().contains(e.to.port));
                    node_inputs[e.to.port].set(spec.path, value);

                    res.updated_inputs.insert(e.to);
                }
            }
        },
        spec.io);
}

auto ComputationThread::invalidate_input(const gc::ParameterSpec& spec)
    -> void
{
    stop();
    skip_ = 0;
    auto& res = computation_.result;

    visit(
        common::Overloads{
            [&](const gc::ExternalInputSpec& i)
            {
                for (const auto& dst :
                     group(computation_.source_inputs.destinations, i.input))
                {
                    res.updated_inputs.insert(dst);
                }
            },
                [&](const gc::NodeOutputSpec& o)
            {
                for (const auto& e : computation_.graph.edges)
                {
                    if (e.from == o.output)
                        res.updated_inputs.insert(e.to);
                }
            }
        },
        spec.io);
}

auto ComputationThread::start_computation()
    -> void
{
    emit queued_start(InheritPriority);
}

auto ComputationThread::stop()
    -> void
{
    if (!isRunning())
        return;

    stop_source_.request_stop();
    wait();
}

auto ComputationThread::on_started()
    -> void
{
    if (skip_ == 0)
        emit running_state_changed(true);
}

auto ComputationThread::on_finished()
    -> void
{
    if (skip_ == 0)
        emit running_state_changed(false);
}

auto ComputationThread::run()
    -> void
{
    stop_source_ = {};

    if (skip_ == 0)
    {
        auto graph_progress =
            [this](gc::NodeIndex inode, double node_progress)
        { emit progress(inode, node_progress); };

        try_compute(graph_progress);
    }
    else
    {
        auto graph_progress =
            [this](gc::NodeIndex inode, double node_progress)
        {};

        for (size_t i=0; i<skip_; ++i)
        {
            set_feedback();
            try_compute(graph_progress);
            if (!ok_)
                break;
        }
    }

    clear_feedback();
}

auto ComputationThread::try_compute(const auto& graph_progress) -> void
{
    ok_ = false;
    try {
        ok_ = compute(
            computation_, stop_source_.get_token(), &graph_progress);
    }
    catch (std::exception& e)
    {
        emit computation_error(QString::fromUtf8(e.what()));
    }
}

auto ComputationThread::clear_feedback() -> void
{
    auto& res = computation_.result;
    res.updated_inputs.clear();
}

auto ComputationThread::set_feedback() -> void
{
    auto& res = computation_.result;

    for (auto& fb : evolution_->feedback)
    {
        const auto& src_end = fb.source_output.output;
        const auto& src_val = group(res.outputs, src_end.node)[src_end.port];
        for (const auto& dst_end : fb.sink_input.inputs)
        {
            auto& dst_val = group(res.inputs, dst_end.node)[dst_end.port];
            dst_val = src_val;
            res.updated_inputs.insert(dst_end);
        }
    }
}
