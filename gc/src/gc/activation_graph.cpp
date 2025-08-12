/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/activation_graph.hpp"
#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"
#include "gc/algorithm_inspector.hpp"
#include "gc/detail/nested_seq.hpp"
#include "gc/simple_graph_util.hpp"
#include "gc/strong_index.hpp"

#include "common/detail/hash.hpp"
#include "common/detail/ind.hpp"
#include "common/format.hpp"
#include "common/overloads.hpp"
#include "common/strong_grouped.hpp"
#include "common/throw.hpp"

#include <optional>
#include <set>
#include <unordered_map>


using namespace gc::literals;

namespace gc {

namespace {

using common::detail::Hash;
using common::detail::Ind;
using common::detail::ScopedInd;

struct HeaderFileExtractor final
{
    const alg::AlgorithmStorage& storage;

    std::set<std::string_view> headers;
    std::set<std::string_view> system_headers;
    std::set<std::string_view> libs;

    explicit HeaderFileExtractor(const alg::AlgorithmStorage& storage):
        storage{ storage }
    {}

    auto operator()(alg::StepIn_Tag) const noexcept
        -> void
    {}

    auto operator()(alg::StepOut_Tag) const noexcept
        -> void
    {}

    template <alg::AlgorithmIdType Id>
    auto operator()(Id)
        -> bool
    { return true; }

    auto operator()(alg::id::HeaderFile id)
        -> bool
    {
        if (id == common::Zero)
            return true;
        const auto& spec = storage(id);
        (spec.system? system_headers: headers).insert(spec.name);
        return true;
    }

    auto operator()(alg::id::Lib id)
        -> bool
    {
        if (id == common::Zero)
            return true;
        const auto& spec = storage(id);
        libs.insert(spec.name);
        return true;
    }
};

struct OutputActivationExtractor final
{
    const alg::AlgorithmStorage& storage;

    std::vector<alg::id::OutputActivation> out_activations;

    explicit OutputActivationExtractor(
                const alg::AlgorithmStorage& storage):
        storage{ storage }
    {}

    auto operator()(alg::StepIn_Tag) const noexcept
        -> void
    {}

    auto operator()(alg::StepOut_Tag) const noexcept
        -> void
    {}

    template <alg::AlgorithmIdType Id>
    auto operator()(Id)
        -> bool
    { return true; }

    auto operator()(alg::id::OutputActivation id)
        -> bool
    {
        out_activations.push_back(id);
        return true;
    }

    auto operator()(alg::id::ReturnOutputActivation id)
        -> bool
    {
        const auto& spec = storage(id);
        out_activations.push_back(spec.activation);
        return true;
    }
};

template <typename F>
struct VarVisitor final
{
    F f;

    VarVisitor(F f):
        f{ f }
    {}

    auto operator()(alg::StepIn_Tag) const noexcept
        -> void
    {}

    auto operator()(alg::StepOut_Tag) const noexcept
        -> void
    {}

    template <alg::AlgorithmIdType Id>
    auto operator()(Id)
        -> bool
    { return true; }

    auto operator()(alg::id::Var id)
        -> bool
    { return f(id); }
};

// ---

using GraphAlgos =
    common::StrongVector<NodeActivationAlgorithms, NodeIndex>;

auto graph_algos(const ActivationGraph& g, alg::AlgorithmStorage& storage)
    -> GraphAlgos
{
    auto result = GraphAlgos{};
    result.reserve(g.nodes.size());
    for (const auto& node : g.nodes)
        result.push_back(node->activation_algorithms(storage));
    return result;
}

// ---

template <typename F>
auto visit_all_vars(F f,
                    const GraphAlgos& algos,
                    const alg::AlgorithmStorage& alg_storage)
    -> void
{
    auto var_visitor = VarVisitor{ f };
    auto var_inspector =
        alg::AlgorithmInspector{ var_visitor, true, alg_storage };
    for (const auto& node_algos : algos)
    {
        var_inspector(node_algos.context);
        var_inspector(node_algos.input_bindings);
        for (const auto& alg : node_algos.algorithms)
            var_inspector(alg.activate);
    }
}

// ---

struct NodeOutputActivation final
{
    NodeIndex node;
    alg::id::OutputActivation activation;
};

using UpstreamActivationMap =
    std::unordered_multimap<EdgeInputEnd, NodeOutputActivation, Hash>;

auto upstream_activations(const ActivationGraph& g,
                          const GraphAlgos& algos,
                          const alg::AlgorithmStorage& alg_storage)
    -> UpstreamActivationMap
{
    auto result = UpstreamActivationMap{};

    using NodeOutputActivations =
        common::StrongGrouped<alg::id::OutputActivation, NodeIndex, Index>;
    auto out_activations = NodeOutputActivations{};

    for (auto node_index : g.nodes.index_range())
    {
        const auto& node = g.nodes[node_index];
        auto visitor = OutputActivationExtractor{ alg_storage };
        auto inspector = alg::AlgorithmInspector{ visitor, true, alg_storage };
        for (auto port : common::index_range<InputPort>(node->input_count()))
        {
            const auto& alg = algos.at(node_index).algorithms.at(port);
            inspector(alg.activate);
        }
        for (auto out_activation : visitor.out_activations)
            add_to_last_group(out_activations, out_activation);
        next_group(out_activations);
    }

    for (const auto& e : g.edges)
    {
        for (auto activation_id : group(out_activations, e.from.node))
        {
            const auto& activation = alg_storage(activation_id);
            if (activation.port != e.from.port)
                continue;
            auto node_out_activation = NodeOutputActivation{
                .node = e.from.node,
                .activation = activation_id
            };
            result.emplace(e.to, node_out_activation);
        }
    }

    return result;
}

// ---

auto check_source_inputs(const ActivationGraph& g,
                         const GraphAlgos& algos,
                         const alg::AlgorithmStorage& alg_storage,
                         const ActivationGraphSourceTypes& source_types,
                         EdgeInputEndSpan ignored_sources)
    -> void
{
    // Collect connected inputs. Source inputs are all other inputs.
    auto connected_inputs = std::unordered_set<EdgeInputEnd, Hash>{};
    for (const auto& e : g.edges)
        connected_inputs.emplace(e.to);

    // Collect source inputs bound to variables.
    auto bound_source_inputs = std::set<EdgeInputEnd>{};
    for (auto inode : algos.index_range())
    {
        const auto& node_alg = algos[inode];
        for (auto input_binding_id : node_alg.input_bindings)
        {
            const auto& input_binding = alg_storage(input_binding_id);
            auto input = EdgeInputEnd{inode, input_binding.port};
            if (!connected_inputs.contains(input))
                bound_source_inputs.emplace(input);
        }
    }

    // Cneck if any "extra" sources are specified; remove eligible specified
    // sources from bound_source_inputs, so that it eventually only contains
    // missing inputs.
    auto extra_source_inputs = std::set<EdgeInputEnd>{};
    for (const auto& to : source_types.destinations.values)
    {
        if (bound_source_inputs.contains(to))
            bound_source_inputs.erase(to);
        else
            extra_source_inputs.insert(to);
    }

    // If some missing inputs should be ignored,
    // these has to be specified in ignored_sources.
    // Remove such inputs from bound_source_inputs
    auto invalid_ignored_sources = std::set<EdgeInputEnd>{};
    for (const auto& to : ignored_sources)
    {
        auto it = bound_source_inputs.find(to);
        if (it == bound_source_inputs.end())
            invalid_ignored_sources.insert(to);
        else
            bound_source_inputs.erase(it);
    }

    // At this point, bound_source_inputs contains missing source inputs only.
    if (extra_source_inputs.empty() &&
        bound_source_inputs.empty() &&
        invalid_ignored_sources.empty())
        // All fine
        return;

    auto s = std::ostringstream{};
    s << "Source input check failed:";
    if (!extra_source_inputs.empty())
        s << "\n- there are extra inputs: "
          << common::format_seq(extra_source_inputs);
    if (!bound_source_inputs.empty())
        s << "\n- there are missing inputs: "
          << common::format_seq(bound_source_inputs);
    if (!invalid_ignored_sources.empty())
        s << "\n- some sources are not eligible to be ignored: "
          << common::format_seq(invalid_ignored_sources);
    common::throw_<std::invalid_argument>(s.str());
}

// ---

auto template_param_name(size_t index)
    -> std::string
{ return common::format('T', index); }

auto activation_func_name(const EdgeInputEnd& input)
    -> std::string
{ return common::format("activate_node_", input.node, "_", input.port); }

auto render_includes(std::ostream& s,
                     const ActivationGraph& g,
                     const GraphAlgos& algos,
                     std::span<alg::id::HeaderFile> extra_headers,
                     const alg::AlgorithmStorage& alg_storage)
    -> void
{
    auto visitor = HeaderFileExtractor{ alg_storage };
    auto inspector = alg::AlgorithmInspector{ visitor, true, alg_storage };
    for (const auto& node_alg : algos)
    {
        for (const auto& port_alg : node_alg.algorithms)
            inspector(port_alg.activate);
        inspector(node_alg.context);
    }

    for (auto extra_header : extra_headers)
        inspector(extra_header);

    auto print_includes =
        [&s](const std::set<std::string_view>& headers,
             char open_mark,
             char close_mark)
    {
        if (headers.empty())
            return;

        for (auto h : headers)
            s << "#include " << open_mark << h << close_mark << '\n';
        s << '\n';
    };

    print_includes(visitor.headers, '"', '"');
    print_includes(visitor.system_headers, '<', '>');

    if (!visitor.libs.empty())
    {
        s << "// Libraries\n";
        for (auto lib: visitor.libs)
            s << "// - " << lib << '\n';
        s << '\n';
    }
}

// ---

using VarTypes =
    std::unordered_map<alg::id::Var, alg::Var, Hash>;

auto is_type_from_binding(const alg::Var& var) -> bool
{
    return holds_alternative<alg::id::TypeFromBinding>(var);
}

class ValueTypeFromBindingResolver
{
public:
    ValueTypeFromBindingResolver(bool& used,
                                 VarTypes& var_source_types,
                                 alg::id::Type source_type,
                                 const EdgeInputEnd& to) :
        used_{used},
        var_source_types_{var_source_types},
        source_type_{source_type},
        to_{to}
    {}

    auto operator()(const alg::InputBinding& input_binding, const alg::Var& var)
        -> void
    {
        used_ = true;

        visit(
            common::Overloads{
                [&](const alg::id::FuncInvocation&)
                {
                    if (source_type_ != common::Zero)
                        common::throw_(
                            "Input ",
                            to_,
                            " has a source type specified, but its eligibility"
                            " cannot be checked. Specify Zero here.");
                },
                [&](const alg::id::Type& original_type)
                {
                    if (source_type_ == common::Zero)
                        return;
                    if (source_type_ != original_type)
                        common::throw_(
                            "Input ",
                            to_,
                            " has a source type specified conflicting with"
                            " node-defined type. Specify Zero here.");
                },
                [&](const alg::id::TypeFromBinding&)
                {
                    if (source_type_ == common::Zero)
                        common::throw_(
                            "Input ",
                            to_,
                            " has no source type specified,"
                            " but a type is required.");
                    if (var_source_types_.contains(input_binding.var))
                        common::throw_(
                            "Input ",
                            to_,
                            " has source type specified more than once");
                    var_source_types_.emplace(input_binding.var, source_type_);
                },
            },
            var);
    }

private:
    bool& used_;
    VarTypes& var_source_types_;
    alg::id::Type source_type_;
    const EdgeInputEnd& to_;
};

auto resolve_variable_types(const ActivationGraph& g,
                            const ActivationGraphSourceTypes& source_types,
                            const GraphAlgos& algos,
                            const alg::AlgorithmStorage& alg_storage)
    -> VarTypes
{
    auto visit_input_bindings_for = [&](const EdgeInputEnd& to, auto f)
    {
        const auto& input_bindings = algos.at(to.node).input_bindings;

        for (auto input_binding_id : input_bindings)
        {
            const auto& input_binding = alg_storage(input_binding_id);
            if (input_binding.port != to.port)
                continue;
            const auto& var = alg_storage(input_binding.var);
            f(input_binding, var);
        }
    };

    // Resolve `TypeFromBinding` from upstream activation data
    using Var2Var =
        std::unordered_multimap<alg::id::Var, alg::id::Var, Hash>;
    auto up_vars = Var2Var{};
    auto up_act = upstream_activations(g, algos, alg_storage);
    for (const auto& [to, upstream] : up_act)
    {
        visit_input_bindings_for(
            to,
            [&](const alg::InputBinding& input_binding,
                const alg::Var& var)
            {
                if (!is_type_from_binding(var))
                    return;
                auto upstream_var = alg_storage(upstream.activation).var;
                up_vars.emplace(input_binding.var, upstream_var);
            });
    }

    // Resolve `TypeFromBinding` from source types
    auto var_source_types = VarTypes{};
    for (auto index : common::index_range(source_types.types.size()))
    {
        auto source_type = source_types.types[index];
        for (const auto& to : group(source_types.destinations, index))
        {
            auto used = false;
            auto resolve_type_from_binding = ValueTypeFromBindingResolver{
                used, var_source_types, source_type, to
            };
            visit_input_bindings_for(to, resolve_type_from_binding);
            if (!used)
                common::throw_(
                    "Destination ", to, " for source type was not used");
        }
    }

    auto result = VarTypes{};

    // Build result from `up_vars` and `var_source_types`
    auto n_up_vars = up_vars.size();
    for (const auto& [k, v] : up_vars)
    {
        // Find all root ancestors of `k`
        decltype(n_up_vars) i_up_var = 0;
        auto current_v = std::vector<alg::id::Var>{ v };
        auto next_v = std::vector<alg::id::Var>{};
        for (; i_up_var<n_up_vars; ++i_up_var)
        {
            auto advanced = false;
            next_v.clear();
            for (auto v_ : current_v)
            {
                auto it = up_vars.find(v_);
                if (it == up_vars.end())
                    next_v.push_back(v_);
                else
                {
                    next_v.push_back(it->second);
                    advanced = true;
                }
            }
            if (!advanced)
                break;
            std::swap(current_v, next_v);
        }
        if (i_up_var == n_up_vars)
            common::throw_(
                "Circular loop in upstream acivations, starting at var ", k);

        auto spec = std::optional<alg::Var>{};
        for (auto v_ : current_v)
        {
            auto v_spec = alg_storage(v_);
            if (holds_alternative<alg::id::TypeFromBinding>(v_spec))
            {
                // Type must be in `var_source_types`
                auto it = var_source_types.find(v_);
                if (it == var_source_types.end())
                    common::throw_(
                        "Variable ", v_,
                        " of type `TypeFromBinding` is unbound");
                v_spec = it->second;
            }
            if (!spec.has_value())
                spec = v_spec;
            else
                common::throw_(
                    "Variable ", k, " has ancestors (",
                    common::format_seq(current_v),
                    ") some of which have different types");
        }
        assert(spec.has_value());
        result.emplace(k, *spec);
    }

    // Make sure that all variables of type `TypeFromBinding` are bound,
    // using var_source_types for any remaining unbound vars
    auto complete_binding = [&]( alg::id::Var id )
    {
        const auto& spec = alg_storage(id);
        if (!holds_alternative<alg::id::TypeFromBinding>(spec))
            return false;
        if (result.contains(id))
            return false;
        if (auto it = var_source_types.find(id); it != var_source_types.end())
        {
            result.emplace(id, it->second);
            return false;
        }
        common::throw_(
            "Variable ", id, " of type `TypeFromBinding` is unbound");
    };
    visit_all_vars(complete_binding, algos, alg_storage);

    return result;
}

// ---

class VarTypeRenderer final
{
public:
    VarTypeRenderer(std::ostream& s,
                    const VarTypes& var_types,
                    const alg::AlgorithmStorage& alg_storage):
        s_{ s },
        var_types_{ var_types },
        alg_storage_{ alg_storage }
    {}

    auto operator()(alg::id::Var id) const
        -> void
    {
        current_id_ = id;

        s_ << "using T" << id << " =\n    ";
        visit(*this, alg_storage_(id));
        s_ << ";\n\n";

        current_id_ = invalid_var_id_;
    }

    auto operator()(alg::id::FuncInvocation id) const
        -> void
    {
        const auto& i_spec = alg_storage_(id);
        const auto& f_spec = alg_storage_(i_spec.func);
        const auto& a_spec = alg_storage_(i_spec.args);
        s_ << "std::decay_t<decltype("
           << f_spec.name << "("
           << common::format_seq(
                  a_spec,
                  ", ",
                  [](std::ostream& s, alg::id::Var id)
                  { s << "std::declval<T" << id << ">()"; } )
            << "))>";
    }

    auto operator()(alg::id::Type id) const
        -> void
    { s_ << alg_storage_(id).name; }

    auto operator()(alg::id::TypeFromBinding) const
        -> void
    { visit(*this, var_types_.at(current_id_)); }

private:
    std::ostream& s_;
    const VarTypes& var_types_;
    const alg::AlgorithmStorage& alg_storage_;

    static constexpr auto invalid_var_id_ = alg::id::Var{~0u};
    mutable alg::id::Var current_id_ = invalid_var_id_;
};

auto render_var_types(std::ostream& s,
                      const VarTypes& var_types,
                      const GraphAlgos& algos,
                      const alg::AlgorithmStorage& alg_storage)
    -> void
{
    SimpleGraph<alg::id::Var> var_dep;

    auto extract_var_dep = [&](alg::id::Var id, const alg::Var& spec)
    {
        if (!holds_alternative<alg::id::FuncInvocation>(spec))
            return;
        auto invocation = std::get<alg::id::FuncInvocation>(spec);
        const auto& invocation_spec = alg_storage(invocation);
        const auto& args_spec = alg_storage(invocation_spec.args);
        for (auto arg : args_spec)
            var_dep.edges.push_back(simple_edge(arg, id));
    };

    visit_all_vars(
        [&](alg::id::Var id)
        {
            var_dep.nodes.push_back(id);
            extract_var_dep(id, alg_storage(id));
            return false;
        },
        algos, alg_storage);

    for (const auto& [id, spec] : var_types)
        extract_var_dep(id, spec);

    auto vars_sorted = topological_sort(var_dep);

    s << "\n// Variable types\n\n";

    auto renderer = VarTypeRenderer{ s, var_types, alg_storage };
    for (auto id : vars_sorted)
        renderer(id);
}

auto render_context_type(std::ostream& s,
                         const ActivationGraph& g,
                         const GraphAlgos& algos,
                         const alg::AlgorithmStorage& alg_storage)
    -> alg::Vars
{
    auto result = alg::Vars{};

    s << "\n// Context type\n"
         "struct Context final\n{\n";

    auto render_decl = [&](alg::id::Var id)
    {
        s << "    T" << id << " var_" << id << ";\n";
        result.push_back(id);
    };

    for (auto node_index : algos.index_range())
    {
        const auto* node = g.nodes[node_index].get();
        const auto& node_alg = algos[node_index];
        s << "    // Node " << node_index << " (" << node->type_name() << ")\n";

        for (const auto& input_binding : node_alg.input_bindings)
            render_decl(alg_storage(input_binding).var);
        if (node_alg.context != common::Zero)
            for (auto id : alg_storage(node_alg.context))
                render_decl(id);
        if (algos.index_range().contains(node_index + NodeCount{1}))
            s << "\n";
    }

    s << "};\n\n";
    return result;
}

class AlgorithmRenderer final
{
public:

    AlgorithmRenderer(std::ostream& s,
                      const ActivationGraph& g,
                      const GraphAlgos& algos,
                      NodeIndex node_index,
                      const alg::AlgorithmStorage& storage,
                      const alg::Vars& context_vars):
        visitor_{ s, g, algos, node_index, storage, context_vars },
        inspector_{ visitor_, false, storage }
    {}

    auto operator()(alg::id::Statement activate)
        -> void
    { inspector_(activate); }

private:
    struct Visitor;
    using Inspector = alg::AlgorithmInspector<Visitor>;

    struct Visitor final
    {
        Visitor(
            std::ostream& s,
            const ActivationGraph& g,
            const GraphAlgos& algos,
            NodeIndex node_index,
            const alg::AlgorithmStorage& storage,
            const alg::Vars& context_vars):
                s_{ s },
                g_{ g },
                algos_{ algos },
                node_index_{ node_index },
                storage_{ storage },
                context_vars_{ context_vars.begin(), context_vars.end() },
                fmt_{ *this }
        {
            // Extract activations from specified node

            std::vector<Edge> filtered_edges;
            std::ranges::copy_if(
                g.edges,
                back_inserter(filtered_edges),
                [&](const Edge& e){ return e.from.node == node_index; });
            std::ranges::sort(filtered_edges);

            auto output_count = g.nodes.at(node_index)->output_count();
            auto it = filtered_edges.begin();
            for (auto output_port : index_range<OutputPort>(output_count))
            {
                for (; it != filtered_edges.end() &&
                       it->from.port == output_port; ++it)
                    common::add_to_last_group(activations_, it->to);
                next_group(activations_);
            }
        }

        auto operator()(alg::StepIn_Tag) noexcept
            -> void
        { ++ind_; }

        auto operator()(alg::StepOut_Tag) noexcept
            -> void
        { --ind_; }

        auto operator()(alg::id::Assign id)
            -> bool
        {
            auto sc = OptionalScope{ *this, ind_ == common::Zero };
            const auto& spec = storage_(id);
            print(fmt_(spec.dst), " = ", fmt_(spec.src), ';');
            return false;
        }

        auto operator()(alg::id::Block id)
            -> bool
        {
            auto relax = Relax{ relaxed_, true };
            auto sc = OptionalScope{ *this, true };
            const auto& spec = storage_(id);
            decl_vars(spec.vars);
            for (auto stmt_id : spec.statements)
                (*this)(stmt_id);
            return false;
        }

        auto decl_vars(alg::id::Vars id)
            -> void
        {
            if (id == common::Zero)
                return;
            for (auto var_id : storage_(id))
            {
                const auto& var_spec = storage_(var_id);
                print("auto ", fmt_(var_id), " = ", visit(fmt_, var_spec), ';');
            }
        }

        auto operator()(alg::id::Do id)
            -> bool
        {
            const auto& spec = storage_(id);
            auto sc = OptionalScope{ *this, ind_.v < 1 ||
                                     spec.vars != common::Zero  ||
                                     !is_relaxed() };
            decl_vars(spec.vars);
            print("do");
            std::optional<ScopedInd> scoped_ind;
            auto relax = Relax{ relaxed_, false };
            if (!is_block(spec.body))
                scoped_ind.emplace(ind_);
            (*this)(spec.body);
            scoped_ind.reset();
            print("while (", fmt_(spec.condition), ");");
            return false;
        }

        auto operator()(alg::id::For id)
            -> bool
        {
            const auto& spec = storage_(id);
            auto sc = OptionalScope{ *this, ind_.v < 1 ||
                                     spec.vars != common::Zero
                                     || !is_relaxed() };
            decl_vars(spec.vars);
            print("for (",
                  fmt_(spec.init), "; ",
                  fmt_(spec.condition), "; ",
                  fmt_(spec.increment),
                  ")");
            std::optional<ScopedInd> scoped_ind;
            auto relax = Relax{ relaxed_, false };
            if (!is_block(spec.body))
                scoped_ind.emplace(ind_);
            (*this)(spec.body);
            return false;
        }

        auto operator()(alg::id::FuncInvocation id)
            -> bool
        {
            auto sc = OptionalScope{ *this, ind_ == common::Zero };
            print(fmt_(id), ';');
            return false;
        }

        auto operator()(alg::id::HeaderFile id)
            -> bool
        {
            // const auto& spec = storage_(id);
            print("// header_file");
            return true;
        }

        auto operator()(alg::id::If id)
            -> bool
        {
            const auto& spec = storage_(id);
            auto sc =
                OptionalScope{ *this, ind_.v < 1
                               || spec.vars != common::Zero
                               || !is_relaxed() };
            decl_vars(spec.vars);
            auto relax = Relax{ relaxed_, false };
            print("if (", fmt_(spec.condition), ")");
            {
                std::optional<ScopedInd> scoped_ind;
                if (!is_block(spec.then_clause))
                    scoped_ind.emplace(ind_);
                (*this)(spec.then_clause);
            }
            if (spec.else_clause != common::Zero)
            {
                print("else");
                std::optional<ScopedInd> scoped_ind;
                if (!is_block(spec.else_clause))
                    scoped_ind.emplace(ind_);
                (*this)(spec.else_clause);

            }
            return false;
        }

        auto operator()(alg::id::InputBinding id)
            -> bool
        {
            // const auto& spec = storage_(id);
            print("// input binding");
            return true;
        }

        auto operator()(alg::id::Lib id)
            -> bool
        {
            // const auto& spec = storage_(id);
            print("// lib");
            return true;
        }

        auto operator()(alg::id::OutputActivation id)
            -> bool
        {
            auto sc =
                OptionalScope{ *this, ind_ == common::Zero || !is_relaxed() };

            const auto& spec = storage_(id);

            print("// Activate output port ", spec.port);
            for (auto to : group(activations_, spec.port))
            {
                const auto& bindings = algos_.at(to.node).input_bindings;
                auto it = std::find_if(
                    bindings.begin(),
                    bindings.end(),
                    [&](const alg::id::InputBinding& id)
                      { return storage_(id).port == to.port; } );
                print("// Activate ", edge({node_index_, spec.port}, to));
                if (it != bindings.end())
                {
                    // Copy variable
                    // TODO: we may want to reduce copying in the future
                    print(fmt_(storage_(*it).var), " = ", fmt_(spec.var), ';');
                }
                print(activation_func_name({to.node, to.port}), "(ctx);");
            }
            return true;
        }

        auto operator()(alg::id::ReturnOutputActivation id)
            -> bool
        {
            // const auto& spec = storage_(id);
            print("// return output activation");
            return true;
        }

        auto operator()(alg::id::Statement id)
            -> bool
        {
            visit(*this, storage_(id));
            return false;
        }

        auto operator()(alg::id::Symbol id)
            -> bool
        {
            print("// symbol");
            return true;
        }

        auto operator()(alg::id::Type id)
            -> bool
        {
            print("// type");
            return true;
        }

        auto operator()(alg::id::TypeFromBinding id)
            -> bool
        {
            print("// type from binding");
            return true;
        }

        auto operator()(alg::id::Var id)
            -> bool
        {
            print("// var");
            return true;
        }

        auto operator()(alg::id::Vars id)
            -> bool
        {
            print("// vars");
            return true;
        }

        auto operator()(alg::id::While id)
            -> bool
        {
            const auto& spec = storage_(id);
            auto sc =
                OptionalScope{ *this, ind_.v < 1 ||
                               spec.vars != common::Zero ||
                               !is_relaxed() };
            decl_vars(spec.vars);
            print("while (", fmt_(spec.condition), ")");
            std::optional<ScopedInd> scoped_ind;
            auto relax = Relax{ relaxed_, false };
            if (!is_block(spec.body))
                scoped_ind.emplace(ind_);
            (*this)(spec.body);
            return false;
        }

        template <typename... Ts>
        auto print(Ts&&... args) const
            -> void
        {
            s_ << ind_
              << common::format(std::forward<Ts>(args)...)
              << '\n';
        }

        auto is_block(alg::id::Statement id) const
            -> bool
        {
            const auto& spec = storage_(id);
            return visit(IsBlock{is_relaxed()}, spec);
        }

        struct IsBlock final
        {
            bool relaxed;
            auto operator()(alg::id::Assign) const noexcept -> bool
            { return false; }
            auto operator()(alg::id::FuncInvocation) const noexcept -> bool
            { return false; }
            auto operator()(alg::id::OutputActivation) const noexcept -> bool
            { return !relaxed; }
            auto operator()(alg::id::If) const noexcept -> bool
            { return !relaxed; }
            auto operator()(alg::id::For) const noexcept -> bool
            { return !relaxed; }
            auto operator()(alg::id::While) const noexcept -> bool
            { return !relaxed; }
            auto operator()(alg::id::Do) const noexcept -> bool
            { return !relaxed; }
            auto operator()(alg::id::Block) const noexcept -> bool
            { return true; }
        };

        struct Formatter final
        {
            Visitor& visitor_;

            auto operator()(alg::id::Var id) const
                -> std::string
            {
                if (visitor_.context_vars_.contains(id))
                    return common::format("ctx.var_", id);

                // TODO: Make sure var is found in `scopes_`
                return common::format("var_", id);
            }

            auto operator()(const alg::AssignRhs& rhs) const
                -> std::string
            { return visit(*this, rhs); }

            auto operator()(alg::id::FuncInvocation id) const
                -> std::string
            {
                const auto& spec = visitor_.storage_(id);
                const auto& f_spec = visitor_.storage_(spec.func);
                const auto& a_spec = visitor_.storage_(spec.args);
                return
                    common::format(f_spec.name, "(") +
                    common::format_seq(
                        a_spec,
                        ", ",
                        [&](std::ostream& s, alg::id::Var id)
                            { s << (*this)(id); }) +
                    ")";
            }

            auto operator()(alg::id::Type id) const
                -> std::string
            {
                const auto& spec = visitor_.storage_(id);
                return common::format(spec.name, "{}"); // TODO: Is this always what we need?
            }

            auto operator()(alg::id::TypeFromBinding) const
                -> std::string
            { return "Type from binding"; }
        };

        struct OptionalScope final
        {
            Visitor& visitor_;
            std::optional<ScopedInd> scoped_ind_;

            OptionalScope(Visitor& visitor, bool need_scope):
                visitor_{ visitor }
            {
                if (need_scope)
                {
                    visitor_.print('{');
                    scoped_ind_.emplace(visitor_.ind_);
                }
            }

            ~OptionalScope()
            {
                if (scoped_ind_.has_value())
                {
                    scoped_ind_.reset();
                    visitor_.print('}');
                }
            }
        };

        std::ostream& s_;
        const ActivationGraph& g_;
        const GraphAlgos& algos_;
        NodeIndex node_index_;
        const alg::AlgorithmStorage& storage_;
        std::unordered_set<alg::id::Var, Hash> context_vars_;
        Ind ind_{};
        std::vector<alg::id::Vars> scopes_;

        Formatter fmt_;

        common::StrongGrouped<EdgeInputEnd, OutputPort, Index> activations_;


        struct Relax final
        {
            std::vector<bool>& relaxed_;
            explicit Relax(std::vector<bool>& relaxed, bool relax)
                : relaxed_{ relaxed }
            { relaxed_.push_back(relax); }
            ~Relax()
            { relaxed_.pop_back(); }
        };
        auto is_relaxed() const -> bool
        { return relaxed_.back(); }
        std::vector<bool> relaxed_;
    };

    Visitor visitor_;
    Inspector inspector_;
};

auto render_activation_algorithm(std::ostream& s,
                                 const ActivationGraph& g,
                                 const GraphAlgos& algos,
                                 NodeIndex node_index,
                                 InputPort port,
                                 const alg::AlgorithmStorage& alg_storage,
                                 const alg::Vars& context_vars)
    -> void
{
    s << "auto " << activation_func_name({node_index, port})
      << "(Context& ctx)\n"
      << "    -> void\n";

    auto renderer =
        AlgorithmRenderer{ s, g, algos, node_index, alg_storage, context_vars };

    const auto& node_alg = algos[node_index];
    const auto& port_alg = node_alg.algorithms[port];

    renderer(port_alg.activate);

    s << '\n';
}

auto generate_nodes(std::ostream& s,
                    const ActivationGraph& g,
                    const ActivationGraphSourceTypes& source_types,
                    const GraphAlgos& algos,
                    const alg::AlgorithmStorage& alg_storage)
    -> void
{
    auto var_types =
        resolve_variable_types(g, source_types, algos, alg_storage);

    render_var_types(s, var_types, algos, alg_storage);
    auto context_vars = render_context_type(s, g, algos, alg_storage);

    s << "\n// Node algorithms\n\n";

    // Start with forward declarations of all functions
    for (auto node_index : algos.index_range())
    {
        const auto& node_alg = algos[node_index];
        for (auto port : node_alg.algorithms.index_range())
            s << "auto " << activation_func_name({node_index, port})
              << "(Context& ctx)\n"
              << "    -> void;\n";
    }

    s << '\n';

    // Now generate function bodes
    for (auto node_index : algos.index_range())
    {
        const auto* node = g.nodes[node_index].get();
        const auto& node_alg = algos[node_index];
        s << "// Node " << node_index << " (" << node->type_name() << ")\n\n";

        for (auto port : node_alg.algorithms.index_range())
            render_activation_algorithm(
                s,
                g,
                algos,
                node_index,
                port,
                alg_storage,
                context_vars);
    }
}


// Context manipulation code generation

template <typename Cb>
auto for_each_node(const ActivationGraph& g, const GraphAlgos& algos, Cb&& cb)
    -> void
{
    for (auto node_index : algos.index_range())
    {
        const auto* node = g.nodes[node_index].get();
        const auto& node_alg = algos[node_index];

        cb(node_index, node, node_alg);
    }
}

template <typename Cb>
auto for_each_input_binding(
    NodeIndex node_index,
    const NodeActivationAlgorithms& node_alg,
    const alg::AlgorithmStorage& alg_storage,
    Cb&& cb) -> void
{
    for (const auto& input_binding_id : node_alg.input_bindings)
    {
        const auto& input_binding = alg_storage(input_binding_id);
        auto input = EdgeInputEnd{node_index, input_binding.port};
        cb(input, input_binding.var);
    }
}

template <typename Cb>
auto for_each_input_binding(
    const ActivationGraph& g,
    const GraphAlgos& algos,
    const alg::AlgorithmStorage& alg_storage,
    Cb&& cb) -> void
{
    for_each_node(
        g,
        algos,
        [&](NodeIndex node_index,
            const ActivationNode* node,
            const NodeActivationAlgorithms& node_alg)
        {
            for_each_input_binding(node_index, node_alg, alg_storage, cb);
        });
}

template <typename Cb>
auto for_each_state_var(
    NodeIndex node_index,
    const NodeActivationAlgorithms& node_alg,
    const alg::AlgorithmStorage& alg_storage,
    Cb&& cb) -> void
{
    if (node_alg.context == common::Zero)
        return;

    const auto& context = alg_storage(node_alg.context);
    for (auto var_index : common::index_range<size_t>(context.size()))
    {
        auto var = context[var_index];
        cb(node_index, var_index, var);
    }
}

template <typename Cb>
auto for_each_state_var(
    const ActivationGraph& g,
    const GraphAlgos& algos,
    const alg::AlgorithmStorage& alg_storage,
    Cb&& cb) -> void
{
    for_each_node(
        g,
        algos,
        [&](NodeIndex node_index,
            const ActivationNode* node,
            const NodeActivationAlgorithms& node_alg)
        {
            for_each_state_var(node_index, node_alg, alg_storage, cb);
        });
}

auto generate_context_util(std::ostream& s,
                           const ActivationGraph& g,
                           const ActivationGraphSourceTypes& source_types,
                           const GraphAlgos& algos,
                           const alg::AlgorithmStorage& alg_storage) -> void
{
    s << R"(
// Context manipulation

constexpr auto combined_u32_and_u8(uint32_t node, uint8_t port) -> uint64_t
{ return uint64_t{node} << 8 | port; }

using ContextManager = agc_rt::ContextManager<Context>;

extern "C" {

auto create_context() -> agc_rt::ContextHandle*
{ return ContextManager::create_context(); }

auto delete_context(agc_rt::ContextHandle* h) -> void
{ ContextManager::delete_context(h); }


)";

    auto input_func_name = [](const EdgeInputEnd& input)
    {
        return common::format(
            "context_input_var_", input.node.v, '_', int{input.port.v});
    };

    auto compressed_edge_expr = [](const EdgeInputEnd& input)
    {
        return common::format(
            "combined_u32_and_u8(", input.node.v, ", ", int{input.port.v}, ')');
    };

    auto compressed_var_expr = [](NodeIndex node_index, size_t var_index)
    {
        return common::format(
            "combined_u32_and_u8(", node_index.v, ", ", var_index, ')');
    };

    auto state_func_name = [](NodeIndex node_index, size_t state_index)
    {
        return common::format(
            "context_state_var_", node_index.v, '_', state_index);
    };

    for_each_node(
        g, algos,
        [&](NodeIndex node_index,
            const ActivationNode* node,
            const NodeActivationAlgorithms& node_alg)
        {
            s << "// Context variables for node " << node_index
              << " (" << node->type_name() << ")\n\n";

            for_each_input_binding(
                node_index,
                node_alg,
                alg_storage,
                [&](const EdgeInputEnd& input, alg::id::Var var)
                {
                    s << "auto " << input_func_name(input)
                      << "(agc_rt::ContextHandle* h) -> T"
                      << var
                      << R"(*
{
  auto& ctx = *ContextManager::context_from_handle(h);
  return &ctx.var_)" << var << R"(;
}

)";
                });

        for_each_state_var(
            node_index,
            node_alg,
            alg_storage,
            [&](NodeIndex node_index, size_t var_index, alg::id::Var var)
            {
                s << "auto " << state_func_name(node_index, var_index)
                  << "(agc_rt::ContextHandle* h) -> T"
                  << var
                  << R"(*
{
  auto& ctx = *ContextManager::context_from_handle(h);
  return &ctx.var_)" << var << R"(;
}

)";
            });

        });


    s << R"(
auto get_context_input_var(agc_rt::ContextHandle* h,
                           uint64_t input_edge_end,
                           std::any& value)
    -> void
{
    switch (input_edge_end)
    {
)";

    for_each_input_binding(
        g,
        algos,
        alg_storage,
        [&](const EdgeInputEnd& input_end, alg::id::Var var)
        {
            s << "    case " << compressed_edge_expr(input_end) << ":\n"
              "        value = *" << input_func_name(input_end) << R"((h);
        break;
)";
        });
    s << R"(    default:
        throw std::invalid_argument(
            "get_context_var_for_port: Unknown input edge");
    }
}

)";

    s << R"(
auto set_context_input_var(agc_rt::ContextHandle* h,
                           uint64_t input_edge_end,
                           const std::any& value)
    -> void
{
    switch (input_edge_end)
    {
)";
    for_each_input_binding(
        g,
        algos,
        alg_storage,
        [&](const EdgeInputEnd& input_end, alg::id::Var var)
        {
            s << "    case " << compressed_edge_expr(input_end) << ":\n"
              "        *" << input_func_name(input_end)
              << "(h) = std::any_cast<const T" << var << R"(&>(value);
        break;
)";
        });
    s << R"(    default:
        throw std::invalid_argument(
            "set_context_var_for_port: Unknown input edge");
    }
}

)";

    s << R"(
auto get_context_state_var(agc_rt::ContextHandle* h,
                           uint32_t node_index,
                           uint8_t state_index,
                           std::any& value)
    -> void
{
    switch (combined_u32_and_u8(node_index, state_index))
    {
)";
    for_each_state_var(
        g,
        algos,
        alg_storage,
        [&](NodeIndex node_index, size_t var_index, alg::id::Var var)
        {
            s << "    case " << compressed_var_expr(node_index, var_index)
              << ":\n"
              << "        value = *" <<
             state_func_name(node_index, var_index) << R"((h);
        break;
)";
        });

    s << R"(    default:
        throw std::invalid_argument(
            "get_context_var_for_state: Unknown node and state index pair");
    }
}
)";

    s << R"(
auto set_context_state_var(agc_rt::ContextHandle* h,
                           uint32_t node_index,
                           uint8_t state_index,
                           const std::any& value)
    -> void
{
    switch (combined_u32_and_u8(node_index, state_index))
    {
)";
    for_each_state_var(
        g,
        algos,
        alg_storage,
        [&](NodeIndex node_index, size_t var_index, alg::id::Var var)
        {
            s << "    case " << compressed_var_expr(node_index, var_index)
              << ":\n"
              "        *" << state_func_name(node_index, var_index)
              << "(h) = std::any_cast<const T" << var << R"(&>(value);
        break;
)";
        });
    s << R"(
    default:
        throw std::invalid_argument(
            "set_context_var_for_state: Unknown node and state index pair");
    }
}

} // extern "C"
)";
}


// Graph activation analysis

using RequiredInputs = std::unordered_map<EdgeInputEnd, InputPorts, Hash>;
using ActivationSeq = detail::NestedSequence<EdgeInputEnd>;
using InputActivations = std::unordered_map<EdgeInputEnd, ActivationSeq, Hash>;

struct GraphActivation final
{
    RequiredInputs required_inputs;
    InputActivations input_activations;
};

class GraphActivationBuilder final
{
public:
    static auto build(const ActivationGraph& g,
                      const ActivationGraphSourceTypes& src_types,
                      const GraphAlgos& algos,
                      const alg::AlgorithmStorage& alg_storage)
        -> GraphActivation
    {
        auto emap = EdgeMap{};

        auto required_inputs = RequiredInputs{};
        auto node_activations = InputActivations{};

        // build edge map
        for (const auto& edge : g.edges)
            emap.emplace(edge.from, edge.to);

        // Extract activation sequences for all input ports of all nodes
        for (auto inode : algos.index_range())
        {
            const auto& node_algos = algos[inode];
            for (auto port : node_algos.algorithms.index_range())
            {
                auto input = EdgeInputEnd{ inode, port };
                const auto& port_algo = node_algos.algorithms[port];
                required_inputs.emplace(input, port_algo.required_inputs);
                auto visitor = Visitor{ inode, emap, alg_storage };
                visitor(port_algo.activate);
                node_activations.emplace(
                    input, std::move(visitor).activation_seq());
            }
        }

        // Build source input activations by extending `node_activations`
        auto input_activations = InputActivations{};
        assert(group_count(src_types.destinations) == src_types.types.size());
        for (auto source_group : group_indices(src_types.destinations))
            for (const auto& to : group(src_types.destinations, source_group))
            {
                auto passed_inputs = InputSet{};
                auto& seq = input_activations[to];
                auto sc = ScopedSeq{ seq, AllInOrder };
                seq.push_back(to);
                extend_activation(seq,
                                  passed_inputs,
                                  node_activations, to);
            }

        return { std::move(required_inputs), std::move(input_activations) };
    }

private:
    using EdgeMap = std::unordered_multimap<EdgeOutputEnd, EdgeInputEnd, Hash>;
    using ScopedSeq = gc::detail::ScopedNestedSequence<EdgeInputEnd>;
    static constexpr auto AllInOrder = gc::detail::SeqType::AllInOrder;
    static constexpr auto Every = gc::detail::SeqType::Every;
    using InputSet = std::unordered_set<EdgeInputEnd, Hash>;

    class Visitor final
    {
    public:
        Visitor(NodeIndex inode,
                const EdgeMap& emap,
                const alg::AlgorithmStorage& alg_storage):
            inode_{ inode },
            emap_{ emap },
            s_{ alg_storage }
        {}

        auto operator()(alg::id::Statement id)
            -> void
        {
            auto sc = ScopedSeq{seq_, AllInOrder};
            std::visit(*this, s_(id));
        }

        auto operator()(alg::id::Assign id)
            -> void
        {}

        auto operator()(alg::id::Block id)
            -> void
        {
            auto sc = ScopedSeq{seq_, AllInOrder};
            const auto& spec = s_(id);
            for (auto statement_id : spec.statements)
                (*this)(statement_id);
        }

        auto operator()(alg::id::FuncInvocation id)
            -> void
        {}

        auto operator()(alg::id::OutputActivation id)
            -> void
        {
            const auto& spec = s_(id);
            auto r = emap_.equal_range({inode_, spec.port});
            for (auto it=r.first; it!=r.second; ++it)
                seq_.push_back(it->second);
        }

        auto operator()(alg::id::If id)
            -> void
        {
            const auto& spec = s_(id);
            assert(spec.then_clause != common::Zero);
            if (spec.else_clause == common::Zero)
                (*this)(spec.then_clause);
            else
            {
                auto sc = ScopedSeq{seq_, Every};
                (*this)(spec.then_clause);
                (*this)(spec.else_clause);
            }
        }

        auto operator()(alg::id::For id)
            -> void
        {
            const auto& spec = s_(id);
            (*this)(spec.body);
        }

        auto operator()(alg::id::While id)
            -> void
        {
            const auto& spec = s_(id);
            (*this)(spec.body);
        }

        auto operator()(alg::id::Do id)
            -> void
        {
            const auto& spec = s_(id);
            (*this)(spec.body);
        }

        auto activation_seq() && noexcept -> ActivationSeq
        { return std::move(seq_); }

    private:
        NodeIndex inode_;
        const EdgeMap& emap_;
        const alg::AlgorithmStorage& s_;
        ActivationSeq seq_;
    };

    static auto extend_activation(ActivationSeq& result,
                                  InputSet& passed_inputs,
                                  const InputActivations& node_activations,
                                  const EdgeInputEnd& e)
        -> void
    {
        assert(!passed_inputs.contains(e));
        passed_inputs.emplace(e);
        const auto& activation_seq = node_activations.at(e);
        for (const auto& item : activation_seq)
        {
            if (!std::holds_alternative<EdgeInputEnd>(item))
            {
                result.push_back(item);
                continue;
            }
            const auto& next_e = std::get<EdgeInputEnd>(item);
            if (passed_inputs.contains(next_e))
                continue;
            result.push_back(next_e);
            extend_activation(result, passed_inputs, node_activations, next_e);
        }
    }
};

auto graph_activation(const ActivationGraph& g,
                      const ActivationGraphSourceTypes& src_types,
                      const GraphAlgos& algos,
                      const alg::AlgorithmStorage& alg_storage)
    -> GraphActivation
{ return GraphActivationBuilder::build(g, src_types, algos, alg_storage); }

auto generate_entry_point(std::ostream& s,
                          const ActivationGraph& g,
                          const ActivationGraphSourceTypes& source_types,
                          const GraphAlgos& algos,
                          const alg::AlgorithmStorage& alg_storage)
    -> void
{
    auto ga = graph_activation(g, source_types, algos, alg_storage);

    using InputState = std::unordered_map<EdgeInputEnd, bool, Hash>;
    class P final
    {
    public:
        explicit P(const GraphActivation& ga):
            ga_{ &ga }
        {
            for (const auto& [e, _] : ga.required_inputs)
                s_[e] = false;
        }

        auto operator()(const EdgeInputEnd& e)
            -> bool
        {
            const auto& rq = ga_->required_inputs.at(e);
            for (auto port : rq)
            {
                if (port == e.port)
                    // Activation will deliver this input
                    continue;

                if (!s_.at({e.node, port}))
                    // Required input is missing
                    return false;
            }
            s_.at(e) = true;
            return true;
        }

    private:
        const GraphActivation* ga_;
        InputState s_;
    };

    auto activation_order = std::vector<EdgeInputEnd>{};
    activation_order.reserve(ga.input_activations.size());
    auto passed_inputs = std::unordered_set<EdgeInputEnd, Hash>{};
    auto p = P{ ga };
    while (activation_order.size() != ga.input_activations.size())
    {
        auto progress = false;
        for (const auto& [input, seq] : ga.input_activations)
        {
            if (passed_inputs.contains(input))
                continue;

            if (detail::test_sequence(seq, p, p))
            {
                passed_inputs.insert(input);
                activation_order.push_back(input);
                progress = true;
            }
        }
        if (!progress)
            common::throw_("Failed to order graph inputs"); // TODO: More details
    }

    s << R"(
extern "C" auto entry_point(agc_rt::ContextHandle* h)
    -> void
{
   auto& ctx = *ContextManager::context_from_handle(h);

)";

    // Activate inputs according to `activation_order`
    for (const auto& e : activation_order)
        s << "  " << activation_func_name(e) << "(ctx);\n";

    s <<
         "}\n";
}

auto add_agc_rt_lib(std::vector<alg::id::HeaderFile>& extra_headers,
                    alg::AlgorithmStorage& s) -> void
{
    auto lib = s(gc::alg::Lib{ .name = "agc_rt" });

    auto add_header = [&](std::string name)
    {
        extra_headers.push_back(
            s(alg::HeaderFile{ .name = name, .lib = lib }));
    };

    add_header("agc_rt/context_util.hpp");
}

auto add_common_system_headers(std::vector<alg::id::HeaderFile>& extra_headers,
                               alg::AlgorithmStorage& s) -> void
{
    auto add_header = [&](std::string name)
    {
        extra_headers.push_back(
            s(alg::HeaderFile{ .name = name, .system = true }));
    };

    add_header("any");
    add_header("cstdint");
    add_header("stdexcept");
}

} // anonymous namespace


auto generate_source(std::ostream& s,
                     const ActivationGraph& g,
                     alg::AlgorithmStorage& alg_storage,
                     const ActivationGraphSourceTypes& source_types,
                     EdgeInputEndSpan ignored_sources)
    -> void
{
    // Find external inputs

    // Determine external input activation sequence
    auto algos = graph_algos(g, alg_storage);

    // deBUG, TODO: Remove
    std::cout << "==== GRAPH ALGORITHMS ====\n";
    for (auto inode : g.nodes.index_range())
    {
        const auto* node = g.nodes[inode].get();
        std::cout << "---- Node " << inode
                  << " - " << node->meta().type_name
                  << " [" << common::format_seq(node->input_names()) << " / "
                  << common::format_seq(node->output_names()) << ']'
                  << " ---- \n";
        std::cout
            << PrintableNodeActivationAlgorithms{algos.at(inode), alg_storage}
            << '\n';
    }
    std::cout << "========\n\n";

    // Check that all source inputs bound to variables
    // are present in source_types
    check_source_inputs(g, algos, alg_storage, source_types, ignored_sources);

    // Add the agc_rt library
    auto extra_headers = std::vector<alg::id::HeaderFile>{};
    add_agc_rt_lib(extra_headers, alg_storage);

    // Add headers used by context access API
    add_common_system_headers(extra_headers, alg_storage);

    // Generate #includes
    render_includes(s, g, algos, extra_headers, alg_storage);

    // Generate code for graph nodes
    generate_nodes(s, g, source_types, algos, alg_storage);

    // Generate context manipulation functions
    generate_context_util(s, g, source_types, algos, alg_storage);

    // Generate entry point
    generate_entry_point(s, g, source_types, algos, alg_storage);
}

auto generate_source(std::ostream& s,
                     const ActivationGraph& g)
    -> void
{
    auto alg_storage = alg::AlgorithmStorage{};
    generate_source(s, g, alg_storage);
}

} // namespace gc
