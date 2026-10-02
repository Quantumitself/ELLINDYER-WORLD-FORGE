#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "Core/ApplicationInfo.hpp"
#include "Core/BuildInfo.hpp"
#include "Core/Settings/ApplicationSettings.hpp"
#include "Project/ProjectManager.hpp"

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

    [[nodiscard]] const std::filesystem::path& GetSettingsFilePath() const noexcept;

    [[nodiscard]] bool AreUserDirectoriesReady() const noexcept;

    [[nodiscard]] ellindyer::core::settings::ApplicationSettingsManager&
        GetSettingsManager() noexcept;

    [[nodiscard]] const ellindyer::core::settings::ApplicationSettingsManager&
        GetSettingsManager() const noexcept;

    [[nodiscard]] ellindyer::project::ProjectManager& GetProjectManager() noexcept;

    [[nodiscard]] const ellindyer::project::ProjectManager& GetProjectManager() const noexcept;

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
    std::filesystem::path              settings_file_path_;
    bool                               user_directories_ready_ = false;

    std::unique_ptr<ellindyer::core::settings::ApplicationSettingsManager>
                                       settings_manager_;
    std::unique_ptr<ellindyer::project::ProjectManager>
                                       project_manager_;
};

} // namespace ellindyer::app
