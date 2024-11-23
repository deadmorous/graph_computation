#pragma once

#include "gc/algorithm_fwd.hpp"
#include "gc/port.hpp"
#include "gc/type_fwd.hpp"

#include <memory>
#include <variant>
#include <vector>


namespace gc {

using Vars = std::vector<id::Var>;

struct FuncArgSpec final
{
    const gc::Type* type;
    bool ref;
};

struct FuncSignature final
{
    const gc::Type* ret;
    std::vector<FuncArgSpec> args;
};

struct Func final
{
    std::string name;
    FuncSignature signature;
};

struct InputBinding
{
    gc::InputPort port;
    id::Var var;
};

struct FuncInvocation final
{
    id::Func func;
    id::Var result;
    id::Vars args;
};

struct If final
{
    id::Vars vars;
    id::FuncInvocation condition;
    id::Statement then_clause;
    id::Statement else_clause;
};

struct For final
{
    id::Vars vars;
    id::FuncInvocation init;
    id::FuncInvocation condition;
    id::FuncInvocation increment;
    id::Statement body;
};

struct While final
{
    id::Vars vars;
    id::FuncInvocation condition;
    id::Statement body;
};

struct Do final
{
    id::Vars vars;
    id::FuncInvocation condition;
    id::Statement body;
};

struct Block
{
    id::Vars vars;
    std::vector<id::Statement> statements;
};

using Statement = std::variant<
    id::FuncInvocation,
    id::OutputActivation,
    id::If,
    id::For,
    id::While,
    id::Do,
    id::Block>;

class AlgorithmStorage
{
public:
    AlgorithmStorage();
    ~AlgorithmStorage();

    auto new_var() -> id::Var;
    auto operator()(Vars) -> id::Vars;
    auto operator()(Func) -> id::Func;
    auto operator()(InputBinding) -> id::InputBinding;
    auto operator()(FuncInvocation) -> id::FuncInvocation;
    auto operator()(If) -> id::If;
    auto operator()(For) -> id::For;
    auto operator()(While) -> id::While;
    auto operator()(Do) -> id::Do;
    auto operator()(Block) -> id::Block;
    auto operator()(Statement) -> id::Statement;

    auto operator()(id::Vars) -> const Vars&;
    auto operator()(id::Func) -> const Func&;
    auto operator()(id::InputBinding) -> const InputBinding&;
    auto operator()(id::FuncInvocation) -> const FuncInvocation&;
    auto operator()(id::If) -> const If&;
    auto operator()(id::For) -> const For&;
    auto operator()(id::While) -> const While&;
    auto operator()(id::Do) -> const Do&;
    auto operator()(id::Block) -> const Block&;
    auto operator()(id::Statement) -> const Statement&;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace gc
