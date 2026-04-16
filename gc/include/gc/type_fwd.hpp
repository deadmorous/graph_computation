/** @file
 * @brief Compatibility shim — redirects to mpk::mix::value type system fwd decls.
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "common/enum_flags.hpp"
#include "common/enum_type.hpp"
#include "common/struct_type.hpp"
#include "common/type.hpp"

#include "mpk/mix/value/type_fwd.hpp"


namespace gc {

using mpk::mix::value::ScalarTypeId;
using mpk::mix::value::StringTypeId;
using mpk::mix::value::SetTypeId;
using mpk::mix::value::AggregateType;

using mpk::mix::value::scalar_type_id_of;
using mpk::mix::value::string_type_id_of;
using mpk::mix::value::set_type_id_of;

using mpk::mix::value::ScalarType;
using mpk::mix::value::StringType;

using mpk::mix::value::CustomTypeToId;
using mpk::mix::value::IdToCustomType;
using mpk::mix::value::EnumTypeToId;
using mpk::mix::value::IdToEnumType;

using mpk::mix::value::RegisteredCustomType;
using mpk::mix::value::RegisteredEnumType;

using mpk::mix::value::Type;

} // namespace gc
