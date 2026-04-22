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
        [&]<typename T, typename AggT>(mpk::mix::Type_Tag<T> tag,
                                       mpk::mix::Type_Tag<AggT>)
    {
        const auto* type = type_of(tag);
        auto name = AggT{type}.name();
        result.register_value(name, type);
    };
    register_type( mpk::mix::Type<bool       >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<std::byte  >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<float      >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<double     >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<int8_t     >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<int16_t    >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<int32_t    >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<int64_t    >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<uint8_t    >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<uint16_t   >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<uint32_t   >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<uint64_t   >, mpk::mix::Type<ScalarT> );
    register_type( mpk::mix::Type<std::string>, mpk::mix::Type<StringT> );

    result.register_value("ValuePath", type_of<ValuePath>());
}


} // namespace gc
