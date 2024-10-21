#pragma once
#include "common/macro.hpp"

#define GCLIB_STRUCT_TYPE_FIELD_OF(x, field) \
    x.field

#define GCLIB_STRUCT_TYPE_FIELD_NAME(_, field) #field

#define GCLIB_STRUCT_TYPE_FIELD_TYPE(Class, field) \
    decltype(Class::field)

#define GCLIB_STRUCT_TYPE_FIELD_TYPE_REF(Class, field) \
    decltype(Class::field)&

#define GCLIB_STRUCT_TYPE_FIELD_TYPE_CREF(Class, field) \
    const decltype(Class::field)&

#define GCLIB_STRUCT_TYPE(Struct, ...)                                      \
    constexpr inline auto fields_of(Struct& x)                              \
        -> std::tuple<                                                      \
            GCLIB_MAP_COMMA_SEP_LIST(                                       \
                GCLIB_STRUCT_TYPE_FIELD_TYPE_REF, Struct, ##__VA_ARGS__) >  \
    { return {                                                              \
            GCLIB_MAP_COMMA_SEP_LIST(                                       \
                GCLIB_STRUCT_TYPE_FIELD_OF, x, ##__VA_ARGS__) }; }          \
                                                                            \
    constexpr inline auto fields_of(const Struct& x)                        \
        -> std::tuple<                                                      \
            GCLIB_MAP_COMMA_SEP_LIST(                                       \
                GCLIB_STRUCT_TYPE_FIELD_TYPE_CREF, Struct, ##__VA_ARGS__) > \
    { return {                                                              \
            GCLIB_MAP_COMMA_SEP_LIST(                                       \
                GCLIB_STRUCT_TYPE_FIELD_OF, x, ##__VA_ARGS__) }; }          \
                                                                            \
    auto fields_of(Struct&& x) = delete;                                    \
                                                                            \
    constexpr inline auto tuple_tag_of(common::Type_Tag<Struct>)            \
        -> common::Type_Tag<std::tuple<                                     \
            GCLIB_MAP_COMMA_SEP_LIST(                                       \
                GCLIB_STRUCT_TYPE_FIELD_TYPE, Struct, ##__VA_ARGS__) >>     \
    { return {}; }                                                          \
                                                                            \
    constexpr inline auto field_names_of(common::Type_Tag<Struct>)          \
        -> std::array<std::string_view, GCLIB_NUM_ARGS(__VA_ARGS__)>        \
    { return {                                                              \
            GCLIB_MAP_COMMA_SEP_LIST(                                       \
                GCLIB_STRUCT_TYPE_FIELD_NAME, _, ##__VA_ARGS__) }; }        \
    static_assert(true)
