#include "Core/ApplicationInfo.hpp"

#include <sstream>

namespace ellindyer::core
{

ApplicationInfo GetApplicationInfo()
{
    ApplicationInfo info{};
    info.name         = std::string(GetApplicationName());
    info.tagline      = std::string(GetApplicationTagline());
    info.organization = std::string(GetApplicationOrganization());
    info.version      = GetApplicationVersionString();
    info.build        = GetBuildInfo();
    return info;
}

std::string GetApplicationInfoSummary()
{
    const ApplicationInfo info = GetApplicationInfo();
    std::ostringstream stream;
    stream << info.name << ' ' << info.version << '\n';
    stream << info.tagline << '\n';
    stream << GetBuildInfoSummary();
    return stream.str();
}

} // namespace ellindyer::core
