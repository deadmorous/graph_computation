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
                const ElementFormatter& element_formatter = {})
    -> std::string
{
    std::ostringstream s;
    std::string_view delim = "";
    for (const auto& item : seq)
    {
        s << delim;
        element_formatter(s, item);
        delim = ",";
    }
    return s.str();
}

} // namespace common
