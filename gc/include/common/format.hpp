/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

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

struct DefaultFormatter
{
    template <typename T>
    auto operator()(std::ostream& s, const T& value) const
        -> void
    { s << value; }
};

template <typename Seq, typename ElementFormatter = DefaultFormatter>
auto format_seq(const Seq& seq,
                std::string_view delim = ",",
                const ElementFormatter& element_formatter = {})
    -> std::string
{
    std::ostringstream s;
    std::string_view current_delim = "";
    for (const auto& item : seq)
    {
        s << current_delim;
        element_formatter(s, item);
        current_delim = delim;
    }
    return s.str();
}

} // namespace common
