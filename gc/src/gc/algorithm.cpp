#include "gc/algorithm.hpp"

#include "common/detail/hash.hpp"

#include <unordered_map>

namespace gc::alg {

namespace {

using common::detail::Hash;

template <typename K, typename V>
using SimpleMap = std::unordered_map<K, V, Hash>;

template <typename K, typename V>
auto store(size_t& next_id, SimpleMap<K, V>& m, V v)
    -> K
{
    auto k = K{ next_id++ };
    m.emplace(k, std::move(v));
    return k;
}

template <typename K, typename V>
auto lookup(const SimpleMap<K, V>& m, const K& k)
    -> const V&
{ return m.at(k); }

template <typename K, typename V>
struct InternedMap
{
    std::unordered_map<K, V, Hash> k2v;
    std::unordered_map<V, K, Hash> v2k;
};

template <typename K, typename V>
auto store(size_t& next_id, InternedMap<K, V>& m, V v)
    -> K
{
    if (auto it = m.v2k.find(v); it != m.v2k.end())
        return it->second;
    auto k = K{ next_id++ };
    m.v2k.emplace(v, k);
    m.k2v.emplace(k, std::move(v));
    return k;
}

template <typename K, typename V>
auto lookup(const InternedMap<K, V>& m, const K& k)
    -> const V&
{ return m.k2v.at(k); }

} // anonymous namespace


struct AlgorithmStorage::Impl
{
    WeakId next_id_{1};

    SimpleMap<id::Assign, Assign> assign_;
    SimpleMap<id::Block, Block> block_;
    SimpleMap<id::Do, Do> do_;
    SimpleMap<id::For, For> for_;
    SimpleMap<id::FuncInvocation, FuncInvocation> func_invocation_;
    InternedMap<id::HeaderFile, HeaderFile> header_file_;
    SimpleMap<id::If, If> if_;
    InternedMap<id::InputBinding, InputBinding> input_binding_;
    InternedMap<id::Lib, Lib> lib_;
    InternedMap<id::OutputActivation, OutputActivation> output_activation_;
    InternedMap<id::OutputBinding, OutputBinding> output_binding_;
    InternedMap<id::ReturnOutputActivation, ReturnOutputActivation>
        return_output_activation_;
    SimpleMap<id::Statement, Statement> statement_;
    InternedMap<id::Symbol, Symbol> symbol_;
    InternedMap<id::Type, Type> type_;
    SimpleMap<id::TypeFromBinding, TypeFromBinding> type_from_binding_;
    SimpleMap<id::Var, Var> var_;
    InternedMap<id::Vars, Vars> vars_;
    SimpleMap<id::While, While> while_;
};

AlgorithmStorage::AlgorithmStorage() :
    impl_{ std::make_unique<Impl>() }
{}

AlgorithmStorage::~AlgorithmStorage() = default;

#define IMPL_ALGORITHM_STORAGE_METHODS(Type, map_)                          \
    auto AlgorithmStorage::operator()(Type spec)                            \
        -> id::Type                                                         \
    { return store(impl_->next_id_, impl_->map_, std::move(spec)); }        \
                                                                            \
    auto AlgorithmStorage::operator()(id::Type spec_id) const               \
        -> const Type&                                                      \
    { return lookup(impl_->map_, spec_id); }                                \
    static_assert(true)

IMPL_ALGORITHM_STORAGE_METHODS(Assign, assign_);
IMPL_ALGORITHM_STORAGE_METHODS(Block, block_);
IMPL_ALGORITHM_STORAGE_METHODS(Do, do_);
IMPL_ALGORITHM_STORAGE_METHODS(For, for_);
IMPL_ALGORITHM_STORAGE_METHODS(FuncInvocation, func_invocation_);
IMPL_ALGORITHM_STORAGE_METHODS(HeaderFile, header_file_);
IMPL_ALGORITHM_STORAGE_METHODS(If, if_);
IMPL_ALGORITHM_STORAGE_METHODS(InputBinding, input_binding_);
IMPL_ALGORITHM_STORAGE_METHODS(Lib, lib_);
IMPL_ALGORITHM_STORAGE_METHODS(OutputActivation, output_activation_);
IMPL_ALGORITHM_STORAGE_METHODS(OutputBinding, output_binding_);
IMPL_ALGORITHM_STORAGE_METHODS(ReturnOutputActivation,
                               return_output_activation_);
IMPL_ALGORITHM_STORAGE_METHODS(Statement, statement_);
IMPL_ALGORITHM_STORAGE_METHODS(Symbol, symbol_);
IMPL_ALGORITHM_STORAGE_METHODS(Type, type_);
IMPL_ALGORITHM_STORAGE_METHODS(TypeFromBinding, type_from_binding_);
IMPL_ALGORITHM_STORAGE_METHODS(Var, var_);
IMPL_ALGORITHM_STORAGE_METHODS(Vars, vars_);
IMPL_ALGORITHM_STORAGE_METHODS(While, while_);

} // namespace gc::alg
