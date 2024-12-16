#include "gc/activation_graph.hpp"
#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"

#include <algorithm>
#include <unordered_set>


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
    std::unordered_set<alg::id::HeaderFile, Hash> header_files;

    template <alg::AlgorithmIdType Id, typename Spec>
    auto operator()(Id, const Spec&)
        -> bool
    { return true; }

    auto operator()(alg::id::HeaderFile id, const alg::HeaderFile&)
        -> bool
    {
        header_files.insert(id);
        return false;
    }
};





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

auto generate_includes(std::ostream& s,
                       const ActivationGraph& g,
                       const GraphAlgos& algos,
                       const gc::alg::AlgorithmStorage& alg_storage)
    -> void
{
    auto visitor = HeaderFileExtractor{};
    auto inspector = AlgorithmInspector{ visitor, true, alg_storage };
    for (const auto& node_alg : algos)
        for (const auto& port_alg : node_alg.algorithms)
        {
            inspector(port_alg.activate);
            // inspector(port_alg.context);
        }

    auto headers = std::vector<std::string_view>{};
    auto system_headers = std::vector<std::string_view>{};
    for (auto id : visitor.header_files)
    {
        const auto& spec = alg_storage(id);
        (spec.system? system_headers: headers).push_back(spec.name);
    }
    std::sort(headers.begin(), headers.end());
    std::sort(system_headers.begin(), system_headers.end());

    for (auto h : headers)
        s << "#include \"" << h << "\"\n";
    s << '\n';
    for (auto h : system_headers)
        s << "#include <" << h << ">\n";
    s << '\n';
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
    generate_includes(s, g, algos, alg_storage);

    // Generate code for graph nodes

    // Generate entry point

    std::cout << "TODO\n";
}

} // namespace gc
