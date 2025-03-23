/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/alg_known_types.hpp"

#include "gc/algorithm.hpp"

#include "common/throw.hpp"

#include <algorithm>


namespace gc::alg {

namespace {

enum class TypeIndex {
    int_type,
    uint64_t_type,
    double_type,
    size_t_type };

auto index(TypeLiteral t)
    ->TypeIndex
{
    constexpr TypeLiteral literals[] = {
        int_type,
        uint64_t_type,
        double_type,
        size_t_type };
    auto it = std::find(std::begin(literals), std::end(literals), t);
    if (it == std::end(literals))
        common::throw_<std::invalid_argument>(
            "Type literal '", t, "' is not well-known");
    return static_cast<TypeIndex>(it - std::begin(literals));
}

} // anonymous namespace


auto well_known_type(TypeLiteral t, AlgorithmStorage& s)
    -> id::Type
{
    switch(index(t))
    {
    case TypeIndex::int_type:
        return s(Type{.name = "int"});
    case TypeIndex::uint64_t_type:
        return s(Type{
            .name = "uint64_t",
            .header_file = s(HeaderFile{
                .name = "cstdint",
                .system = true }) });
    case TypeIndex::double_type:
        return s(Type{.name = "double"});
    case TypeIndex::size_t_type:
        return s(Type{
            .name = "double",
            .header_file = s(HeaderFile{
                .name = "cstddef",
                .system = true }) });
    }
    __builtin_unreachable();
}

} // namespace gc::alg
