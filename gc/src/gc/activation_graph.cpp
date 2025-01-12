#include "gc/activation_graph.hpp"
#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"
#include "gc/algorithm_inspector.hpp"

#include "common/detail/hash.hpp"
#include "common/format.hpp"
#include "common/throw.hpp"

#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>


using namespace gc::literals;

namespace gc {

namespace {

using common::detail::Hash;

struct HeaderFileExtractor
{
    const gc::alg::AlgorithmStorage& storage;

    std::set<std::string_view> headers;
    std::set<std::string_view> system_headers;
    std::set<std::string_view> libs;

    explicit HeaderFileExtractor(const gc::alg::AlgorithmStorage& storage):
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

auto render_pod(std::ostream& s,
                std::string_view name,
                std::span<alg::id::Var> fields,
                const gc::alg::AlgorithmStorage& alg_storage)
    -> void
{
    std::vector<size_t> type_param_fields;
    for (auto index : common::index_range(fields.size()))
    {
        const auto& f_spec = alg_storage(fields[index]);
        if (std::holds_alternative<alg::id::TypeFromBinding>(f_spec))
            type_param_fields.push_back(index);
    }

    if (!type_param_fields.empty())
    {
        s << "template <"
          << common::format_seq(
            type_param_fields,
            ", ",
            [](std::ostream& s, size_t index){
                s << "typename T" << index;
            })
          << ">\n";
    }
    s << "struct " << name << " final\n{\n";
    for (auto index : common::index_range(fields.size()))
    {
        auto f_id = fields[index];
        const auto& f_spec = alg_storage(f_id);
        if (std::holds_alternative<alg::id::FuncInvocation>(f_spec))
            common::throw_(
                "Deduction of variable type from initializer is not supported "
                "for context variables - ", f_id);
        if (std::holds_alternative<alg::id::Type>(f_spec))
        {
            const auto& t_spec = alg_storage(std::get<alg::id::Type>(f_spec));
            s << "    " << t_spec.name;
        }
        else
        {
            assert(std::holds_alternative<alg::id::TypeFromBinding>(f_spec));
            s << "    T" << index;
        }
        s << " var_" << f_id << ";\n";
    }
    s << "};\n\n";
}

auto render_includes(std::ostream& s,
                       const ActivationGraph& g,
                       const GraphAlgos& algos,
                       const gc::alg::AlgorithmStorage& alg_storage)
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

using DeducedVariableTypes =
    std::unordered_map<gc::alg::id::Var, gc::alg::id::Var, Hash>;

auto resolve_variable_types(const ActivationGraph& g,
                            const ActivationGraphSourceTypes& source_types,
                            const GraphAlgos& algos,
                            const gc::alg::AlgorithmStorage& alg_storage)
    -> DeducedVariableTypes
{
    auto result = DeducedVariableTypes{};
    for (const auto& e : g.edges)
    {
        const auto& dst_algos = algos.at(e.to.node);
        for (auto input_binding_id : dst_algos.input_bindings)
        {
            const auto& input_binding = alg_storage(input_binding_id);
            if (input_binding.port != e.to.port)
                continue;
            const auto& var = alg_storage(input_binding.var);
            if (!std::holds_alternative<gc::alg::id::TypeFromBinding>(var))
                continue;


            // Find activations of e.from
        }
    }
    // TODO
    return result;
}

// ---

auto generate_nodes(std::ostream& s,
                    const ActivationGraph& g,
                    const ActivationGraphSourceTypes& source_types,
                    const GraphAlgos& algos,
                    const gc::alg::AlgorithmStorage& alg_storage)
    -> void
{
    s << "\n// Node algorithms\n\n";

    for (auto node_index : algos.index_range())
    {
        const auto& node_alg = algos[node_index];
        s << "// Node " << node_index << "\n\n";

        std::vector<gc::alg::id::Var> context_vars;
        for (const auto& input_binding : node_alg.input_bindings)
            context_vars.push_back(alg_storage(input_binding).var);
        for (const auto& output_binding : node_alg.output_bindings)
            context_vars.push_back(alg_storage(output_binding).var);
        if (node_alg.context != common::Zero)
            for (auto id : alg_storage(node_alg.context))
                context_vars.push_back(id);

        render_pod(s,
                   common::format("Context_", node_index),
                   context_vars,
                   alg_storage);

        for (auto port : node_alg.algorithms.index_range())
        {
            const auto& port_alg = node_alg.algorithms[port];
            // port_alg.
        }
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
    auto alg_storage = gc::alg::AlgorithmStorage{};
    auto algos = graph_algos(g, alg_storage);

    // Generate #includes
    render_includes(s, g, algos, alg_storage);

    // Generate code for graph nodes
    generate_nodes(s, g, source_types, algos, alg_storage);

    // Generate entry point

    std::cout << "TODO\n";
}

} // namespace gc
