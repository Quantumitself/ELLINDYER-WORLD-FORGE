#pragma once

#include <filesystem>
#include <string>

namespace ellindyer::core
{

class ApplicationPaths
{
public:
    ApplicationPaths() = delete;

    [[nodiscard]] static std::filesystem::path GetExecutablePath();

    [[nodiscard]] static std::filesystem::path GetExecutableDirectory();

    [[nodiscard]] static std::filesystem::path GetResourcesDirectory();

    [[nodiscard]] static std::filesystem::path GetIconsDirectory();

    [[nodiscard]] static std::filesystem::path GetFontsDirectory();

    [[nodiscard]] static std::filesystem::path GetThemesDirectory();

    [[nodiscard]] static std::filesystem::path GetUserDataDirectory();

    [[nodiscard]] static std::filesystem::path GetLogsDirectory();

    [[nodiscard]] static std::filesystem::path GetConfigurationDirectory();

    static bool EnsureUserDirectoriesExist();
};

} // namespace ellindyer::core
