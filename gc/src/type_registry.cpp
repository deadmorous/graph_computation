/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc/type_registry.hpp"

#include "gc/type.hpp"
#include "gc/value_path.hpp"

#include <iostream>

namespace gc {

auto populate_gc_type_registry(gc::TypeRegistry& result)
    -> void
{
    auto register_type =
        [&]<typename T, typename AggT>(common::Type_Tag<T> tag,
                                       common::Type_Tag<AggT>)
    {
        const auto* type = type_of(tag);
        auto name = AggT{type}.name();
        result.register_value(name, type);
    };
    register_type( common::Type<bool       >, common::Type<ScalarT> );
    register_type( common::Type<std::byte  >, common::Type<ScalarT> );
    register_type( common::Type<float      >, common::Type<ScalarT> );
    register_type( common::Type<double     >, common::Type<ScalarT> );
    register_type( common::Type<int8_t     >, common::Type<ScalarT> );
    register_type( common::Type<int16_t    >, common::Type<ScalarT> );
    register_type( common::Type<int32_t    >, common::Type<ScalarT> );
    register_type( common::Type<int64_t    >, common::Type<ScalarT> );
    register_type( common::Type<uint8_t    >, common::Type<ScalarT> );
    register_type( common::Type<uint16_t   >, common::Type<ScalarT> );
    register_type( common::Type<uint32_t   >, common::Type<ScalarT> );
    register_type( common::Type<uint64_t   >, common::Type<ScalarT> );
    register_type( common::Type<std::string>, common::Type<StringT> );

    result.register_value("ValuePath", type_of<ValuePath>());
}


} // namespace gc
