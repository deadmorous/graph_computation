#pragma once

#define GCLIB_DECL_LITERAL(type, literal) \
    constexpr inline auto literal = type{#literal}
