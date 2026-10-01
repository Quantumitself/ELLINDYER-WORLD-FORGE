#include "Core/Version.hpp"

#include <cstdio>

#ifndef ELLINDYER_WORLD_FORGE_VERSION_MAJOR
#    define ELLINDYER_WORLD_FORGE_VERSION_MAJOR 0
#endif
#ifndef ELLINDYER_WORLD_FORGE_VERSION_MINOR
#    define ELLINDYER_WORLD_FORGE_VERSION_MINOR 0
#endif
#ifndef ELLINDYER_WORLD_FORGE_VERSION_PATCH
#    define ELLINDYER_WORLD_FORGE_VERSION_PATCH 0
#endif

namespace ellindyer::core
{

Version GetApplicationVersion() noexcept
{
    return Version{
        ELLINDYER_WORLD_FORGE_VERSION_MAJOR,
        ELLINDYER_WORLD_FORGE_VERSION_MINOR,
        ELLINDYER_WORLD_FORGE_VERSION_PATCH
    };
}

std::string GetApplicationVersionString()
{
    const Version v = GetApplicationVersion();
    char buffer[64] = {};
    std::snprintf(buffer, sizeof(buffer), "%d.%d.%d", v.major, v.minor, v.patch);
    return std::string(buffer);
}

std::string_view GetApplicationName() noexcept
{
    return std::string_view{"Ellindyer World Forge"};
}

std::string_view GetApplicationTagline() noexcept
{
    return std::string_view{"World & Game Design Studio"};
}

std::string_view GetApplicationOrganization() noexcept
{
    return std::string_view{"Ellindyer"};
}

} // namespace ellindyer::core
