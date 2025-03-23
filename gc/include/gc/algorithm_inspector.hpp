/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "gc/algorithm.hpp"

#include <unordered_set>


namespace gc::alg {

constexpr inline struct StepIn_Tag final {} StepIn;
constexpr inline struct StepOut_Tag final {} StepOut;

template <typename Visitor>
struct AlgorithmInspector final
{
    Visitor& visitor;
    bool visit_once;
    const AlgorithmStorage& storage;
    std::unordered_set<WeakId> visited;

    explicit AlgorithmInspector(Visitor& visitor,
                                bool visit_once,
                                const AlgorithmStorage& storage):
        visitor{ visitor },
        visit_once{ visit_once },
        storage{ storage }
    {}

    template <alg::AlgorithmIdType Id>
    auto operator()(Id id)
        -> void
    {
        if (visit_once && !visited.insert(id.v).second)
            return;
        if (!visitor(id))
            return;
        if (id == common::Zero)
            return;
        visitor(StepIn);
        const auto& spec = storage(id);
        (*this)(spec);
        visitor(StepOut);
    }


    auto operator()(const AssignRhs& spec)
        -> void
    { std::visit(*this, spec); }

    auto operator()(const Assign& spec)
        -> void
    {
        (*this)(spec.dst);
        std::visit(*this, spec.src);
    }

    auto operator()(const Block& spec)
        -> void
    {
        (*this)(spec.vars);
        for (auto statement_id : spec.statements)
            (*this)(statement_id);
    }

    auto operator()(const Do& spec)
        -> void
    {
        (*this)(spec.vars);
        (*this)(spec.condition);
        (*this)(spec.body);
    }

    auto operator()(const For& spec)
        -> void
    {
        (*this)(spec.vars);
        (*this)(spec.init);
        (*this)(spec.condition);
        (*this)(spec.increment);
        (*this)(spec.body);
    }

    auto operator()(const FuncInvocation& spec)
        -> void
    {
        (*this)(spec.func);
        (*this)(spec.args);
    }

    auto operator()(const HeaderFile& spec)
        -> void
    { (*this)(spec.lib); }

    auto operator()(const If& spec)
        -> void
    {
        (*this)(spec.vars);
        (*this)(spec.condition);
        (*this)(spec.then_clause);
        (*this)(spec.else_clause);
    }

    auto operator()(const InputBinding& spec)
        -> void
    { (*this)(spec.var); }

    auto operator()(const std::vector<id::InputBinding>& vec)
        -> void
    {
        for (auto id : vec)
            (*this)(id);
    }

    auto operator()(const Lib& spec)
        -> void
    {}

    auto operator()(const OutputActivation& spec)
        -> void
    { (*this)(spec.var); }

    auto operator()(const Statement& spec)
        -> void
    { std::visit(*this, spec); }

    auto operator()(const Symbol& spec)
        -> void
    {
        (*this)(spec.header_file);
    }

    auto operator()(const ReturnOutputActivation& spec)
        -> void
    {}

    auto operator()(const Type& spec)
        -> void
    {
        (*this)(spec.header_file);
    }

    auto operator()(const TypeFromBinding& spec)
        -> void
    {}

    auto operator()(const Var& spec)
        -> void
    {
        std::visit(
            [this](auto id){ (*this)(id); },
            spec);
    }

    auto operator()(const Vars& spec)
        -> void
    {
        for (auto id : spec)
            (*this)(id);
    }

    auto operator()(const While& spec)
        -> void
    {
        (*this)(spec.vars);
        (*this)(spec.condition);
        (*this)(spec.body);
    }

};

template <typename Visitor>
AlgorithmInspector(Visitor&, const AlgorithmStorage&)
    -> AlgorithmInspector<Visitor>;

} // namespace gc::alg
