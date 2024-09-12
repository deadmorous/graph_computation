#pragma once

#include <sstream>
#include <string>
#include <string_view>


namespace common {

template <typename... Ts>
auto format(Ts&&... args)
    -> std::string
{
    std::ostringstream s;
    ((s << args), ...);
    return s.str();
}

template <typename Seq>
auto format_seq(const Seq& seq)
    -> std::string
{
    std::ostringstream s;
    std::string_view delim = "";
    for (const auto& item : seq)
        s << delim << item,   delim = ",";
    return s.str();
}

} // namespace common
