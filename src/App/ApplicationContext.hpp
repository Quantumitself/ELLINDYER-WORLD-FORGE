#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "Core/ApplicationInfo.hpp"
#include "Core/BuildInfo.hpp"

namespace ellindyer::app
{

class ApplicationContext
{
public:
    ApplicationContext();
    ~ApplicationContext();

    ApplicationContext(const ApplicationContext&) = delete;
    ApplicationContext& operator=(const ApplicationContext&) = delete;
    ApplicationContext(ApplicationContext&&) noexcept = delete;
    ApplicationContext& operator=(ApplicationContext&&) noexcept = delete;

    [[nodiscard]] const ellindyer::core::ApplicationInfo& GetApplicationInfo() const noexcept;

    [[nodiscard]] const ellindyer::core::BuildInfo& GetBuildInfo() const noexcept;

    [[nodiscard]] const std::filesystem::path& GetExecutableDirectory() const noexcept;

    [[nodiscard]] const std::filesystem::path& GetResourcesDirectory() const noexcept;

    [[nodiscard]] const std::filesystem::path& GetIconsDirectory() const noexcept;

    [[nodiscard]] const std::filesystem::path& GetFontsDirectory() const noexcept;

    [[nodiscard]] const std::filesystem::path& GetThemesDirectory() const noexcept;

    [[nodiscard]] const std::filesystem::path& GetUserDataDirectory() const noexcept;

    [[nodiscard]] const std::filesystem::path& GetLogsDirectory() const noexcept;

    [[nodiscard]] const std::filesystem::path& GetConfigurationDirectory() const noexcept;

    [[nodiscard]] const std::filesystem::path& GetScratchDirectory() const noexcept;

    [[nodiscard]] bool AreUserDirectoriesReady() const noexcept;

private:
    void ResolvePaths();

    ellindyer::core::ApplicationInfo   application_info_;
    ellindyer::core::BuildInfo         build_info_;
    std::filesystem::path              executable_directory_;
    std::filesystem::path              resources_directory_;
    std::filesystem::path              icons_directory_;
    std::filesystem::path              fonts_directory_;
    std::filesystem::path              themes_directory_;
    std::filesystem::path              user_data_directory_;
    std::filesystem::path              logs_directory_;
    std::filesystem::path              configuration_directory_;
    std::filesystem::path              scratch_directory_;
    bool                               user_directories_ready_ = false;
};

} // namespace ellindyer::app
