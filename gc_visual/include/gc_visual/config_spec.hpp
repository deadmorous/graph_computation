#pragma once

#include <cstdint>
#include <string>

namespace gc_visual {

enum class ConfigSpecificationType : uint8_t
{
    Content,
    FileName
};

struct ConfigSpecification
{
    ConfigSpecificationType type;
    std::string spec;
};

} // namespace gc_visual
