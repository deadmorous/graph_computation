#pragma once

#include "gc/algorithm_fwd.hpp"
#include "gc/port.hpp"

#include "common/struct_type_macro.hpp"

#include <memory>
#include <variant>
#include <vector>
#include <string>


namespace gc::alg {

using AssignRhs = std::variant<
    id::FuncInvocation,
    id::Var>;

struct Assign final
{
    id::Var dst;
    AssignRhs src;
};

struct Block final
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
    id::Vars args;
};

struct HeaderFile final
{
    std::string name;
    bool system{};
    id::Lib lib;

    auto operator==(const HeaderFile&) const noexcept -> bool = default;
};
GCLIB_STRUCT_TYPE(HeaderFile, name, system, lib);

struct If final
{
    id::Vars vars;
    id::FuncInvocation condition;
    id::Statement then_clause;
    id::Statement else_clause;
};

struct InputBinding final
{
    gc::InputPort port;
    id::Var var;

    auto operator==(const InputBinding&) const noexcept -> bool = default;
};
GCLIB_STRUCT_TYPE(InputBinding, port, var);

struct Lib final
{
    std::string name;

    auto operator==(const Lib&) const noexcept -> bool = default;
};
GCLIB_STRUCT_TYPE(Lib, name);

struct OutputActivation final
{
    gc::OutputPort port;
    id::Var var;

    auto operator==(const OutputActivation&) const noexcept -> bool = default;
};
GCLIB_STRUCT_TYPE(OutputActivation, port, var);

struct ReturnOutputActivation final
{
    id::OutputActivation activation;

    auto operator==(const ReturnOutputActivation&) const noexcept
        -> bool = default;
};
GCLIB_STRUCT_TYPE(ReturnOutputActivation, activation);

using Statement = std::variant<
    id::Assign,
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

    auto operator==(const Symbol&) const noexcept -> bool = default;
};
GCLIB_STRUCT_TYPE(Symbol, name, header_file);

struct Type final
{
    std::string name;
    id::HeaderFile header_file;

    auto operator==(const Type&) const noexcept -> bool = default;
};
GCLIB_STRUCT_TYPE(Type, name, header_file);

struct TypeFromBinding final
{
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

    auto operator()(Assign) -> id::Assign;
    auto operator()(Block) -> id::Block;
    auto operator()(Do) -> id::Do;
    auto operator()(For) -> id::For;
    auto operator()(FuncInvocation) -> id::FuncInvocation;
    auto operator()(HeaderFile) -> id::HeaderFile;
    auto operator()(If) -> id::If;
    auto operator()(InputBinding) -> id::InputBinding;
    auto operator()(Lib) -> id::Lib;
    auto operator()(OutputActivation) -> id::OutputActivation;
    auto operator()(ReturnOutputActivation) -> id::ReturnOutputActivation;
    auto operator()(Statement) -> id::Statement;
    auto operator()(Symbol) -> id::Symbol;
    auto operator()(Type) -> id::Type;
    auto operator()(TypeFromBinding) -> id::TypeFromBinding;
    auto operator()(Var) -> id::Var;
    auto operator()(Vars) -> id::Vars;
    auto operator()(While) -> id::While;

    auto operator()(id::Assign) const -> const Assign&;
    auto operator()(id::Block) const -> const Block&;
    auto operator()(id::Do) const -> const Do&;
    auto operator()(id::For) const -> const For&;
    auto operator()(id::FuncInvocation) const -> const FuncInvocation&;
    auto operator()(id::HeaderFile) const -> const HeaderFile&;
    auto operator()(id::If) const -> const If&;
    auto operator()(id::InputBinding) const -> const InputBinding&;
    auto operator()(id::Lib) const -> const Lib&;
    auto operator()(id::OutputActivation) const -> const OutputActivation&;
    auto operator()(id::ReturnOutputActivation) const
        -> const ReturnOutputActivation&;
    auto operator()(id::Statement) const -> const Statement&;
    auto operator()(id::Symbol)const -> const Symbol&;
    auto operator()(id::Type) const -> const Type&;
    auto operator()(id::TypeFromBinding) const -> const TypeFromBinding&;
    auto operator()(id::Var) const -> const Var&;
    auto operator()(id::Vars) const -> const Vars&;
    auto operator()(id::While) const -> const While&;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

template <typename T>
concept AlgorithmIdType =
    requires(T id, const AlgorithmStorage& s)
    {
        requires std::same_as<T, std::decay_t<T>>;
        s(id);
    };

} // namespace gc::alg
