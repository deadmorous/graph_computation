#pragma once

#include "common/type.hpp"


namespace common {

template <typename T>
concept StructType =
    requires(T t, const T ct)
{
    fields_of(t);
    fields_of(ct);
    tuple_tag_of(Type<T>);
    field_names_of(Type<T>);
};

} // namespace common
