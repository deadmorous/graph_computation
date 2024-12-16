#include "gc/activation_graph.hpp"
#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"

#include "common/format.hpp"
#include "common/throw.hpp"

#include <algorithm>
#include <set>
#include <unordered_set>


using namespace gc::literals;

namespace gc {

namespace {

template <typename Visitor>
struct AlgorithmInspector
{
    Visitor& visitor;
    bool visit_once;
    const gc::alg::AlgorithmStorage& storage;
    std::unordered_set<alg::WeakId> visited;

    explicit AlgorithmInspector(Visitor& visitor,
                                bool visit_once,
                                const gc::alg::AlgorithmStorage& storage):
        visitor{ visitor },
        visit_once{ visit_once },
        storage{ storage }
    {}

    template <alg::AlgorithmIdType Id>
    auto operator()(Id id)
        -> void
    {
        if (id == common::Zero)
            return;
        if (visit_once && !visited.insert(id.v).second)
            return;
        const auto& spec = storage(id);
        if (!visitor(id, spec))
            return;
        (*this)(spec);
    }


    auto operator()(const alg::Block& spec)
        -> void
    {
        (*this)(spec.vars);
        for (auto statement_id : spec.statements)
            (*this)(statement_id);
    }

    auto operator()(const alg::Do& spec)
        -> void
    {
        (*this)(spec.vars);
        (*this)(spec.condition);
        (*this)(spec.body);
    }

    auto operator()(const alg::For& spec)
        -> void
    {
        (*this)(spec.vars);
        (*this)(spec.init);
        (*this)(spec.condition);
        (*this)(spec.increment);
        (*this)(spec.body);
    }

    auto operator()(const alg::FuncInvocation& spec)
        -> void
    {
        (*this)(spec.func);
        (*this)(spec.result);
        (*this)(spec.args);
    }

    auto operator()(const alg::HeaderFile& spec)
        -> void
    {
        (*this)(spec.lib);
    }

    auto operator()(const alg::If& spec)
        -> void
    {
        (*this)(spec.vars);
        (*this)(spec.condition);
        (*this)(spec.then_clause);
        (*this)(spec.else_clause);
    }

    auto operator()(const alg::InputBinding& spec)
        -> void
    {
        (*this)(spec.var);
    }

    auto operator()(const alg::Lib& spec)
        -> void
    {}

    auto operator()(const alg::OutputActivation& spec)
        -> void
    {
        (*this)(spec.var);
    }

    auto operator()(const alg::Statement& spec)
        -> void
    {
        std::visit(
            [this]<typename Id>(Id id){
                (*this)(id);
            },
            spec);
        // std::visit(*this, spec);
    }

    auto operator()(const alg::Symbol& spec)
        -> void
    {
        (*this)(spec.header_file);
    }

    auto operator()(const alg::Type& spec)
        -> void
    {
        (*this)(spec.header_file);
    }

    auto operator()(const alg::TypeFromBinding& spec)
        -> void
    {}

    auto operator()(const alg::Var& spec)
        -> void
    {
        std::visit(
            [this](auto id){ (*this)(id); },
            spec);
    }

    auto operator()(const alg::Vars& spec)
        -> void
    {
        for (auto id : spec)
            (*this)(id);
    }

    auto operator()(const alg::While& spec)
        -> void
    {
        (*this)(spec.vars);
        (*this)(spec.condition);
        (*this)(spec.body);
    }

};

template <typename Visitor>
AlgorithmInspector(Visitor&, const alg::AlgorithmStorage&)
    -> AlgorithmInspector<Visitor>;


// TODO: Move to a header file
struct Hash
{
    template <common::StrongType T>
    auto operator()(const T& key) const noexcept
        -> size_t
    { return std::hash<typename T::Weak>{}( key.v ); }
};

struct HeaderFileExtractor
{
    std::set<std::string_view> headers;
    std::set<std::string_view> system_headers;
    std::set<std::string_view> libs;

    template <alg::AlgorithmIdType Id, typename Spec>
    auto operator()(Id, const Spec&)
        -> bool
    { return true; }

    auto operator()(alg::id::HeaderFile, const alg::HeaderFile& h)
        -> bool
    {
        (h.system? system_headers: headers).insert(h.name);
        return true;
    }

    auto operator()(alg::id::Lib, const alg::Lib& lib)
        -> bool
    {
        libs.insert(lib.name);
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
    auto visitor = HeaderFileExtractor{};
    auto inspector = AlgorithmInspector{ visitor, true, alg_storage };
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

auto generate_nodes(std::ostream& s,
                    const ActivationGraph& g,
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


auto generate_source(const ActivationGraph& g)
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
    generate_nodes(s, g, algos, alg_storage);

    // Generate entry point

    std::cout << "TODO\n";
}

} // namespace gc
