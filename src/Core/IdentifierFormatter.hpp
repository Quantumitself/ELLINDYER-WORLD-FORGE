#pragma once

#include <string>
#include <string_view>

#include "Core/Identifier.hpp"

namespace ellindyer::core
{

class IdentifierFormatter
{
public:
    IdentifierFormatter() = delete;

    [[nodiscard]] static std::string FormatFull(const Identifier& identifier);

    [[nodiscard]] static std::string FormatShort(const Identifier& identifier);

    [[nodiscard]] static std::string FormatPrefixed(std::string_view prefix,
                                                    const Identifier& identifier);

    [[nodiscard]] static std::string FormatPrefixedShort(std::string_view prefix,
                                                         const Identifier& identifier);
};

} // namespace ellindyer::core
