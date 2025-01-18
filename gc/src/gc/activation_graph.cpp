#include "gc/activation_graph.hpp"
#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"
#include "gc/algorithm_inspector.hpp"
#include "gc/simple_graph_util.hpp"
#include "gc/strong_index.hpp"

#include "common/detail/hash.hpp"
#include "common/detail/ind.hpp"
#include "common/format.hpp"
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

struct HeaderFileExtractor
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

struct OutputActivationExtractor
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
struct VarVisitor
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

auto template_param_name(size_t index)
    -> std::string
{ return common::format('T', index); }

auto activation_func_name(const EdgeInputEnd& input)
    -> std::string
{ return common::format("activate_node_", input.node, "_", input.port); }

auto render_includes(std::ostream& s,
                       const ActivationGraph& g,
                       const GraphAlgos& algos,
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
            if (!holds_alternative<alg::id::TypeFromBinding>(var))
                continue;
            f(input_binding);
        }
    };

    // Resolve `TypeFromBinding` from upstream activation data
    using Var2Var =
        std::unordered_map<alg::id::Var, alg::id::Var, Hash>;
    auto up_vars = Var2Var{};
    auto up_act = upstream_activations(g, algos, alg_storage);
    for (const auto& [to, upstream] : up_act)
    {
        visit_input_bindings_for(
            to,
            [&](const alg::InputBinding& input_binding)
            {
                if (up_vars.contains(input_binding.var))
                    // TODO: Handle this case later
                    common::throw_(
                        "Input ", to,
                        " has more than one activation, we are not yet able"
                        " to deduce the type in this case");
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
            visit_input_bindings_for(
                to,
                [&](const alg::InputBinding& input_binding)
                {
                    if (var_source_types.contains(input_binding.var))
                        common::throw_(
                            "Input ", to,
                            " has source type specified more than once");
                    var_source_types.emplace(input_binding.var, source_type);
                });
        }
    }

    auto result = VarTypes{};

    // Build result from `up_vars` and `var_source_types`
    auto n_up_vars = up_vars.size();
    for (const auto& [k, v] : up_vars)
    {
        // Find the root ancestor of `k`
        decltype(n_up_vars) i_up_var = 0;
        auto v_ = v;
        for (; i_up_var<n_up_vars; ++i_up_var)
        {
            auto it = up_vars.find(v_);
            if (it == up_vars.end())
                break;
            v_ = it->second;
        }
        if (i_up_var == n_up_vars)
            common::throw_(
                "Circular loop in upstream acivations, starting at var ", k);

        const auto& spec = alg_storage(v_);
        if (holds_alternative<alg::id::TypeFromBinding>(spec))
        {
            // Type must be in `var_source_types`
            if (auto it = var_source_types.find(v_); it!=var_source_types.end())
                result.emplace(k, it->second);
            else
                common::throw_(
                    "Variable ", v_, " of type `TypeFromBinding` is unbound");
        }
        else
            result.emplace(k, spec);
    }

    // Check that all variables of type `TypeFromBinding` are bound
    auto check_var = [&]( alg::id::Var id )
    {
        const auto& spec = alg_storage(id);
        if (!holds_alternative<alg::id::TypeFromBinding>(spec))
            return false;
        if (!result.contains(id))
            common::throw_(
                "Variable ", id, " of type `TypeFromBinding` is unbound");
        return false;
    };
    visit_all_vars(check_var, algos, alg_storage);

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
        visitor_{ s, g, algos, node_index, inspector_, storage, context_vars },
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
            Inspector& inspector,
            const alg::AlgorithmStorage& storage,
            const alg::Vars& context_vars):
                s_{ s },
                g_{ g },
                algos_{ algos },
                node_index_{ node_index },
                inspector_{ inspector },
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
            const auto& spec = storage_(id);
            print(fmt_(spec.dst), " = ", fmt_(spec.src));
            return false;
        }

        auto operator()(alg::id::Block id)
            -> bool
        {
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
            // const auto& spec = storage_(id);
            print("// do");
            return true;
        }

        auto operator()(alg::id::For id)
            -> bool
        {
            // const auto& spec = storage_(id);
            print("// for");
            return true;
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
            // const auto& spec = storage_(id);
            print("// if");
            return true;
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
            const auto& spec = storage_(id);

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
                OptionalScope{ *this, ind_.v > 1 && spec.vars != common::Zero };
            decl_vars(spec.vars);
            print("while (", fmt_(spec.condition), ")");
            // TODO: Fix `is_block` for statements that can contain scoped vars
            std::optional<ScopedInd> scoped_ind;
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
        { return std::holds_alternative<alg::id::Block>(storage_(id)); }

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
        Inspector& inspector_;
        const alg::AlgorithmStorage& storage_;
        std::unordered_set<alg::id::Var, Hash> context_vars_;
        Ind ind_{};
        std::vector<alg::id::Vars> scopes_;

        Formatter fmt_;

        common::StrongGrouped<EdgeInputEnd, OutputPort, Index> activations_;
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

} // anonymous namespace


auto generate_source(const ActivationGraph& g,
                     const ActivationGraphSourceTypes& source_types)
    -> void
{
    auto& s = std::cout; // TODO

    // Find external inputs

    // Determine external input activation sequence
    auto alg_storage = alg::AlgorithmStorage{};
    auto algos = graph_algos(g, alg_storage);

    // deBUG, TODO: Remove
    std::cout << "==== GRAPH ALGORITHMS ====\n";
    for (const auto& node_algos : algos)
        std::cout
            << PrintableNodeActivationAlgorithms{node_algos, alg_storage}
            << std::endl;
    std::cout << "========\n\n";

    // Generate #includes
    render_includes(s, g, algos, alg_storage);

    // Generate code for graph nodes
    generate_nodes(s, g, source_types, algos, alg_storage);

    // Generate entry point

    std::cout << "TODO\n";
}

} // namespace gc
