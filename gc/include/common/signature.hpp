#pragma once
// Compatibility shim — use mpk/mix/signature.hpp directly in new code.
#include "mpk/mix/signature.hpp"

namespace common
{
using mpk::mix::MethodOf;
using mpk::mix::MethodWithSignatureOf;
using mpk::mix::ReturnType;
using mpk::mix::ReturnType_t;
using mpk::mix::SignatureOf;
using mpk::mix::SignatureOf_t;
using mpk::mix::SignatureType;
using mpk::mix::arg_0_type;
using mpk::mix::arity;
using mpk::mix::is_method_of;
using mpk::mix::is_method_with_signature_of;
using mpk::mix::is_signature_type;
using mpk::mix::return_type;
using mpk::mix::signature;
} // namespace common
