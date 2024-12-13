#pragma once

#include "gc/algorithm_fwd.hpp"
#include "gc/port.hpp"

#include <memory>
#include <variant>
#include <vector>
#include <string>


namespace gc::alg {

struct Block
{
    id::Vars vars;
    std::vector<id::Statement> statements;
};

struct Do final
{
    id::Vars vars;
    id::FuncInvocation condition;
    id::Statement body;
};

struct For final
{
    id::Vars vars;
    id::FuncInvocation init;
    id::FuncInvocation condition;
    id::FuncInvocation increment;
    id::Statement body;
};

struct FuncInvocation final
{
    id::Symbol func;
    id::Var result;
    id::Vars args;
};

struct HeaderFile final
{
    std::string name;
    bool system{};
    id::Lib lib;
};

struct If final
{
    id::Vars vars;
    id::FuncInvocation condition;
    id::Statement then_clause;
    id::Statement else_clause;
};

struct InputBinding
{
    gc::InputPort port;
    id::Var var;
};

struct Lib final
{
    std::string name;
};

struct OutputActivation
{
    gc::OutputPort port;
    id::Var var;
};

using Statement = std::variant<
    id::FuncInvocation,
    id::OutputActivation,
    id::If,
    id::For,
    id::While,
    id::Do,
    id::Block>;

struct Symbol final
{
    std::string name;
    id::HeaderFile header_file;
};

struct Type final
{
    std::string name;
    id::HeaderFile header_file;
};

using Var = std::variant<
    id::FuncInvocation,
    id::Type,
    id::TypeFromBinding>;

using Vars = std::vector<id::Var>;

struct While final
{
    id::Vars vars;
    id::FuncInvocation condition;
    id::Statement body;
};


class AlgorithmStorage
{
public:
    AlgorithmStorage();
    ~AlgorithmStorage();

    auto operator()(Block) -> id::Block;
    auto operator()(Do) -> id::Do;
    auto operator()(For) -> id::For;
    auto operator()(FuncInvocation) -> id::FuncInvocation;
    auto operator()(HeaderFile) -> id::HeaderFile;
    auto operator()(If) -> id::If;
    auto operator()(InputBinding) -> id::InputBinding;
    auto operator()(Lib) -> id::Lib;
    auto operator()(OutputActivation) -> id::OutputActivation;
    auto operator()(Statement) -> id::Statement;
    auto operator()(Symbol) -> id::Symbol;
    auto operator()(Type) -> id::Type;
    auto operator()(Var) -> id::Var;
    auto operator()(Vars) -> id::Vars;
    auto operator()(While) -> id::While;

    auto operator()(id::Block) const -> const Block&;
    auto operator()(id::Do) const -> const Do&;
    auto operator()(id::For) const -> const For&;
    auto operator()(id::FuncInvocation) const -> const FuncInvocation&;
    auto operator()(id::HeaderFile) const -> const HeaderFile&;
    auto operator()(id::If) const -> const If&;
    auto operator()(id::InputBinding) const -> const InputBinding&;
    auto operator()(id::Lib) const -> const Lib&;
    auto operator()(id::OutputActivation) const -> const OutputActivation&;
    auto operator()(id::Statement) const -> const Statement&;
    auto operator()(id::Symbol)const -> const Symbol&;
    auto operator()(id::Type) const -> const Type&;
    auto operator()(id::Var) const -> const Var&;
    auto operator()(id::Vars) const -> const Vars&;
    auto operator()(id::While) const -> const While&;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace gc::alg
