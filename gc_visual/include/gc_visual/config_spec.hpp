#pragma once

#include <cstdint>
#include <QString>

namespace gc_visual {

enum class ConfigSpecificationType : uint8_t
{
    Content,
    FileName
};

struct ConfigSpecification
{
    ConfigSpecificationType type;
    QString spec;
};

} // namespace gc_visual
