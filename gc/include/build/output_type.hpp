#pragma once

#include <cstdint>


namespace build {

enum class OutputType : uint8_t
{
    Executable,
    SharedObject
};

} // namespace build
