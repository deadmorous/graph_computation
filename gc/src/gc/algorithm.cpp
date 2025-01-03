#include "gc/algorithm.hpp"

#include <unordered_map>

namespace gc::alg {

struct Hash
{
    template <common::StrongType T>
    auto operator()(const T& key) const noexcept
        -> size_t
    { return std::hash<typename T::Weak>{}( key.v ); }
};

struct AlgorithmStorage::Impl
{
    size_t next_id{1};

    auto new_id() noexcept
        -> size_t
    { return next_id++; }

    std::unordered_map<id::Assign, Assign, Hash> assign_;
    std::unordered_map<id::Block, Block, Hash> block_;
    std::unordered_map<id::Do, Do, Hash> do_;
    std::unordered_map<id::For, For, Hash> for_;
    std::unordered_map<id::FuncInvocation, FuncInvocation, Hash>
        func_invocation_;
    std::unordered_map<id::HeaderFile, HeaderFile, Hash> header_file_;
    std::unordered_map<id::If, If, Hash> if_;
    std::unordered_map<id::InputBinding, InputBinding, Hash> input_binding_;
    std::unordered_map<id::Lib, Lib, Hash> lib_;
    std::unordered_map<id::OutputActivation, OutputActivation, Hash>
        output_activation_;
    std::unordered_map<id::OutputBinding, OutputBinding, Hash> output_binding_;
    std::unordered_map<id::ReturnOutputActivation, ReturnOutputActivation, Hash>
        return_output_activation_;
    std::unordered_map<id::Statement, Statement, Hash> statement_;
    std::unordered_map<id::Symbol, Symbol, Hash> symbol_;
    std::unordered_map<id::Type, Type, Hash> type_;
    std::unordered_map<id::TypeFromBinding, TypeFromBinding, Hash>
        type_from_binding_;
    std::unordered_map<id::Var, Var, Hash> var_;
    std::unordered_map<id::Vars, Vars, Hash> vars_;
    std::unordered_map<id::While, While, Hash> while_;
};

AlgorithmStorage::AlgorithmStorage() :
    impl_{ std::make_unique<Impl>() }
{}

AlgorithmStorage::~AlgorithmStorage() = default;

#define IMPL_ALGORITHM_STORAGE_METHODS(Type, map_)                          \
    auto AlgorithmStorage::operator()(Type spec)                            \
        -> id::Type                                                         \
    {                                                                       \
        auto key = id::Type{ impl_->new_id() };                             \
        impl_->map_.emplace( key, std::move(spec) );                        \
        return key;                                                         \
    }                                                                       \
                                                                            \
    auto AlgorithmStorage::operator()(id::Type spec_id) const               \
        -> const Type&                                                      \
    { return impl_->map_.at(spec_id); }                                     \
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
