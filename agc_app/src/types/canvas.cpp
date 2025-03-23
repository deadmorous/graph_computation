/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "agc_app/types/canvas.hpp"
#include "agc_app/util/brightness_char.hpp"


namespace agc_app {

auto operator<<(std::ostream& s, const Canvas<double>& canvas)
    -> std::ostream&
{
    const auto* d = canvas.pixels.data();
    for (uint32_t y=0; y<canvas.size[1]; ++y)
    {
        for (uint32_t x=0; x<canvas.size[0]; ++x, ++d)
            s << brightness_char(*d);
        s << '\n';
    }
    return s;
}

} // namespace agc_app
