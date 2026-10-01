#pragma once

#include <string>
#include <string_view>

namespace ellindyer::core
{

enum class BuildConfiguration
{
    Debug,
    Release,
    RelWithDebInfo,
    MinSizeRel,
    Unknown
};

struct BuildInfo
{
    BuildConfiguration configuration;
    std::string_view compiler_name;
    std::string_view compiler_version;
    std::string_view platform;
    std::string_view architecture;
    std::string_view cpp_standard;
    std::string_view build_date;
    std::string_view build_time;
};

[[nodiscard]] BuildInfo GetBuildInfo() noexcept;

[[nodiscard]] std::string_view GetBuildConfigurationName(BuildConfiguration configuration) noexcept;

[[nodiscard]] std::string GetBuildInfoSummary();

} // namespace ellindyer::core
