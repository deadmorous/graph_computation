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
