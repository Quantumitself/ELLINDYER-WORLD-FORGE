#include "Core/IdentifierFormatter.hpp"

namespace ellindyer::core
{

std::string IdentifierFormatter::FormatFull(const Identifier& identifier)
{
    return identifier.ToString();
}

std::string IdentifierFormatter::FormatShort(const Identifier& identifier)
{
    return identifier.ToShortString();
}

std::string IdentifierFormatter::FormatPrefixed(std::string_view prefix,
                                                const Identifier& identifier)
{
    std::string result;
    result.reserve(prefix.size() + 1 + 18);
    result.append(prefix);
    result.push_back('-');
    result.append(identifier.ToString());
    return result;
}

std::string IdentifierFormatter::FormatPrefixedShort(std::string_view prefix,
                                                     const Identifier& identifier)
{
    std::string result;
    result.reserve(prefix.size() + 1 + 8);
    result.append(prefix);
    result.push_back('-');
    result.append(identifier.ToShortString());
    return result;
}

} // namespace ellindyer::core
