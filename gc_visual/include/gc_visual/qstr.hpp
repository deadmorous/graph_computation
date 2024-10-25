#pragma once

#include "common/format.hpp"

#include <QString>


inline auto qstr(const std::string& s)
    -> QString
{ return QString::fromUtf8(s.c_str()); }

template <typename... Ts>
auto format_qstr(Ts&&... args)
    -> QString
{ return qstr(common::format(std::forward<Ts>(args)...)); }
