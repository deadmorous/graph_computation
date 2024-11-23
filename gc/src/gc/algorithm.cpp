#include "gc/algorithm.hpp"

#include <unordered_map>

namespace gc {

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

    std::unordered_map<id::Vars, Vars, Hash> vars;
    std::unordered_map<id::Func, Func, Hash> func;
    std::unordered_map<id::InputBinding, InputBinding, Hash> input_binding;
    std::unordered_map<id::FuncInvocation, FuncInvocation, Hash> func_invocation;
    std::unordered_map<id::If, If, Hash> if_;
    std::unordered_map<id::For, For, Hash> for_;
    std::unordered_map<id::While, While, Hash> while_;
    std::unordered_map<id::Do, Do, Hash> do_;
    std::unordered_map<id::Block, Block, Hash> block;
    std::unordered_map<id::Statement, Statement, Hash> statement;
};

AlgorithmStorage::AlgorithmStorage() :
    impl_{ std::make_unique<Impl>() }
{}

AlgorithmStorage::~AlgorithmStorage() = default;

auto AlgorithmStorage::new_var()
    -> id::Var
{ return id::Var{ impl_->new_id() }; }

auto AlgorithmStorage::operator()(Vars vars)
    -> id::Vars
{
    auto key = id::Vars{ impl_->new_id() };
    impl_->vars.emplace( key, std::move(vars) );
    return key;
}

auto AlgorithmStorage::operator()(Func func)
    -> id::Func
{
    auto key = id::Func{ impl_->new_id() };
    impl_->func.emplace( key, std::move(func) );
    return key;
}

auto AlgorithmStorage::operator()(InputBinding input_binding)
    -> id::InputBinding
{
    auto key = id::InputBinding{ impl_->new_id() };
    impl_->input_binding.emplace( key, std::move(input_binding) );
    return key;
}

auto AlgorithmStorage::operator()(FuncInvocation func_invocation)
    -> id::FuncInvocation
{
    auto key = id::FuncInvocation{ impl_->new_id() };
    impl_->func_invocation.emplace( key, std::move(func_invocation) );
    return key;
}

auto AlgorithmStorage::operator()(If if_)
    -> id::If
{
    auto key = id::If{ impl_->new_id() };
    impl_->if_.emplace( key, std::move(if_) );
    return key;
}

auto AlgorithmStorage::operator()(For for_)
    -> id::For
{
    auto key = id::For{ impl_->new_id() };
    impl_->for_.emplace( key, std::move(for_) );
    return key;
}

auto AlgorithmStorage::operator()(While while_)
    -> id::While
{
    auto key = id::While{ impl_->new_id() };
    impl_->while_.emplace( key, std::move(while_) );
    return key;
}

auto AlgorithmStorage::operator()(Do do_)
    -> id::Do
{
    auto key = id::Do{ impl_->new_id() };
    impl_->do_.emplace( key, std::move(do_) );
    return key;
}

auto AlgorithmStorage::operator()(Block block)
    -> id::Block
{
    auto key = id::Block{ impl_->new_id() };
    impl_->block.emplace( key, std::move(block) );
    return key;
}

auto AlgorithmStorage::operator()(Statement statement)
    -> id::Statement
{
    auto key = id::Statement{ impl_->new_id() };
    impl_->statement.emplace( key, std::move(statement) );
    return key;
}

auto AlgorithmStorage::operator()(id::Vars vars)
    -> const Vars&
{ return impl_->vars.at(vars); }

auto AlgorithmStorage::operator()(id::Func func)
    -> const Func&
{ return impl_->func.at(func); }

auto AlgorithmStorage::operator()(id::InputBinding input_binding)
    -> const InputBinding&
{ return impl_->input_binding.at(input_binding); }

auto AlgorithmStorage::operator()(id::FuncInvocation func_invocation)
    -> const FuncInvocation&
{ return impl_->func_invocation.at(func_invocation); }

auto AlgorithmStorage::operator()(id::If if_)
    -> const If&
{ return impl_->if_.at(if_); }

auto AlgorithmStorage::operator()(id::For for_)
    -> const For&
{ return impl_->for_.at(for_); }

auto AlgorithmStorage::operator()(id::While while_)
    -> const While&
{ return impl_->while_.at(while_); }

auto AlgorithmStorage::operator()(id::Do do_)
    -> const Do&
{ return impl_->do_.at(do_); }

auto AlgorithmStorage::operator()(id::Block block)
    -> const Block&
{ return impl_->block.at(block); }

auto AlgorithmStorage::operator()(id::Statement statement)
    -> const Statement&
{ return impl_->statement.at(statement); }


} // namespace gc
