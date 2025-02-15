#pragma once

#include <iterator>


namespace agc_app {

inline auto brightness_char_coarse(double brightness)
    -> char
{
    static constexpr char chars[] = "@%#*+=-:. ";
    constexpr auto n = std::size(chars) - 1; // Do not count trailing '\0'
    auto index = brightness <= 0
        ? 0ul
        : brightness >= 1
            ? n-1
            : static_cast<size_t>(brightness*(n-1) + 0.5);
    return chars[index];
}

inline auto brightness_char_fine(double brightness)
    -> char
{
    static constexpr char chars[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/"
                                    "\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

    constexpr auto n = std::size(chars) - 1; // Do not count trailing '\0'
    auto index = brightness <= 0
                     ? 0ul
                     : brightness >= 1
                           ? n-1
                           : static_cast<size_t>(brightness*(n-1) + 0.5);
    return chars[index];
}

inline auto brightness_char(double brightness)
    -> char
{ return brightness_char_coarse(brightness); }

} // namespace agc_app
