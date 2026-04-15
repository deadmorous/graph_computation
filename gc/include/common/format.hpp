/** @file
 * @brief Compatibility shim — use mpk/mix/util/format_seq.hpp directly in new code.
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "mpk/mix/util/format_seq.hpp"

namespace common {

using mpk::mix::DefaultFormatter;

template <typename Seq, typename ElementFormatter = DefaultFormatter>
auto format_seq(const Seq& seq,
                std::string_view delim = ",",
                const ElementFormatter& element_formatter = {})
    -> std::string
{
    return mpk::mix::format_seq(seq, delim, element_formatter);
}

} // namespace common
