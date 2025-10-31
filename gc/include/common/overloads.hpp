/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

// https://github.com/ytsaurus/ytsaurus/blob/main/util/generic/overloaded.h

namespace common {

template <class... Fs>
struct Overloads : Fs...
{
    using Fs::operator()...;
};

template <class... Fs>
Overloads(Fs...) -> Overloads<Fs...>;

} // namespace common
