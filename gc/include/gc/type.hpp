/** @file
 * @brief Compatibility shim — redirects to mpk::mix::value Type and accessors.
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "gc/type_fwd.hpp"
#include "gc/detail/value_component_access.hpp"

#include "mpk/mix/util/impl_tag.hpp"
#include "mpk/mix/strong/strong.hpp"

#include "mpk/mix/value/type.hpp"


// Old registration macros — delegate to the mpk_mix equivalents.
#define GCLIB_IMPL_REGISTER_TYPE(T, id_, MapToId, MapFromId)                \
    MPKMIX_VALUE_IMPL_REGISTER_TYPE(T, id_, MapToId, MapFromId)

#define GCLIB_REGISTER_CUSTOM_TYPE(T, id_)                                  \
    MPKMIX_VALUE_REGISTER_CUSTOM_TYPE(T, id_)

#define GCLIB_REGISTER_ENUM_TYPE(T, id_)                                    \
    MPKMIX_VALUE_REGISTER_ENUM_TYPE(T, id_)


namespace gc {

using mpk::mix::value::type_of;

using mpk::mix::value::ArrayT;
using mpk::mix::value::CustomT;
using mpk::mix::value::EnumT;
using mpk::mix::value::PathT;
using mpk::mix::value::ScalarT;
using mpk::mix::value::StringT;
using mpk::mix::value::SetT;
using mpk::mix::value::StrongT;
using mpk::mix::value::StructT;
using mpk::mix::value::TupleT;
using mpk::mix::value::VectorT;

using mpk::mix::value::visit;

} // namespace gc
