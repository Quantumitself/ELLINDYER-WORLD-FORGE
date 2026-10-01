#pragma once

#include <string>

#include "Core/BuildInfo.hpp"
#include "Core/Version.hpp"

namespace ellindyer::core
{

struct ApplicationInfo
{
    std::string name;
    std::string tagline;
    std::string organization;
    std::string version;
    BuildInfo   build;
};

[[nodiscard]] ApplicationInfo GetApplicationInfo();

[[nodiscard]] std::string GetApplicationInfoSummary();

} // namespace ellindyer::core
