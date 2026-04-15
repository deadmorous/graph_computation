/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "mpk/mix/util/format_sep.hpp"

#include <QString>


namespace plot {

inline auto qstr(const std::string& s)
    -> QString
{ return QString::fromUtf8(s.c_str()); }

template <typename... Ts>
auto format_qstr(Ts&&... args)
    -> QString
{ return qstr(mpk::mix::format_sep("", std::forward<Ts>(args)...)); }

} // namespace plot
