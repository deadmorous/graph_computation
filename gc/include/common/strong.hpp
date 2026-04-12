#pragma once
// Compatibility shim — use mpk/mix/strong/strong.hpp directly in new code.
#include "mpk/mix/strong/strong.hpp"

namespace common
{
// Meta names available via transitive include of old strong.hpp
using mpk::mix::Type_Tag;
using mpk::mix::Type;
// Concepts from strong/fwd.hpp (available via transitive include of strong.hpp)
using mpk::mix::StrongType;
using mpk::mix::StrongArithmeticType;
using mpk::mix::StrongNumericType;
using mpk::mix::StrongCountType;
using mpk::mix::StrongIndexType;
using mpk::mix::StrongStringType;
using mpk::mix::HasViewType;
using mpk::mix::StrongView;
// Class templates and traits
using mpk::mix::StrongTraits;
using mpk::mix::StrongWithDefaultTraits;
using mpk::mix::StrongIdFeatures;
using mpk::mix::StrongCountFeatures;
using mpk::mix::StrongIndexFeatures;
using mpk::mix::StrongStringFeatures;
using mpk::mix::raw;
// Meta names that strong.hpp previously pulled in transitively
using mpk::mix::Nil_Tag;
using mpk::mix::Nil;
using mpk::mix::Zero_Tag;
using mpk::mix::Zero;
} // namespace common

#define GCLIB_STRONG_TYPE(Name, Weak_, ...) \
    MPKMIX_STRONG_TYPE(Name, Weak_, ##__VA_ARGS__)

#define GCLIB_STRONG_TYPE_WITH_DEFAULT(Name, Weak_, Default, ...) \
    MPKMIX_STRONG_TYPE_WITH_DEFAULT(Name, Weak_, Default, ##__VA_ARGS__)

#define GCLIB_STRONG_LITERAL_SUFFIX(Name, suffix) \
    MPKMIX_STRONG_LITERAL_SUFFIX(Name, suffix)

#define GCLIB_STRONG_STRING_VIEW(Name) MPKMIX_STRONG_STRING_VIEW(Name)

#define GCLIB_STRONG_STRING(Name) MPKMIX_STRONG_STRING(Name)
