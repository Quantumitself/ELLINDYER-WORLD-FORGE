#include "App/ApplicationContext.hpp"

#include "Core/ApplicationPaths.hpp"

namespace ellindyer::app
{

ApplicationContext::ApplicationContext()
    : application_info_(ellindyer::core::GetApplicationInfo())
    , build_info_(ellindyer::core::GetBuildInfo())
{
    ResolvePaths();
}

ApplicationContext::~ApplicationContext() = default;

const ellindyer::core::ApplicationInfo& ApplicationContext::GetApplicationInfo() const noexcept
{
    return application_info_;
}

const ellindyer::core::BuildInfo& ApplicationContext::GetBuildInfo() const noexcept
{
    return build_info_;
}

const std::filesystem::path& ApplicationContext::GetExecutableDirectory() const noexcept
{
    return executable_directory_;
}

const std::filesystem::path& ApplicationContext::GetResourcesDirectory() const noexcept
{
    return resources_directory_;
}

const std::filesystem::path& ApplicationContext::GetIconsDirectory() const noexcept
{
    return icons_directory_;
}

const std::filesystem::path& ApplicationContext::GetFontsDirectory() const noexcept
{
    return fonts_directory_;
}

const std::filesystem::path& ApplicationContext::GetThemesDirectory() const noexcept
{
    return themes_directory_;
}

const std::filesystem::path& ApplicationContext::GetUserDataDirectory() const noexcept
{
    return user_data_directory_;
}

const std::filesystem::path& ApplicationContext::GetLogsDirectory() const noexcept
{
    return logs_directory_;
}

const std::filesystem::path& ApplicationContext::GetConfigurationDirectory() const noexcept
{
    return configuration_directory_;
}

const std::filesystem::path& ApplicationContext::GetScratchDirectory() const noexcept
{
    return scratch_directory_;
}

bool ApplicationContext::AreUserDirectoriesReady() const noexcept
{
    return user_directories_ready_;
}

void ApplicationContext::ResolvePaths()
{
    using ellindyer::core::ApplicationPaths;

    executable_directory_    = ApplicationPaths::GetExecutableDirectory();
    resources_directory_     = ApplicationPaths::GetResourcesDirectory();
    icons_directory_         = ApplicationPaths::GetIconsDirectory();
    fonts_directory_         = ApplicationPaths::GetFontsDirectory();
    themes_directory_        = ApplicationPaths::GetThemesDirectory();
    user_data_directory_     = ApplicationPaths::GetUserDataDirectory();
    logs_directory_          = ApplicationPaths::GetLogsDirectory();
    configuration_directory_ = ApplicationPaths::GetConfigurationDirectory();

    if (!user_data_directory_.empty())
    {
        scratch_directory_ = user_data_directory_ / "scratch";
    }

    user_directories_ready_ = ApplicationPaths::EnsureUserDirectoriesExist();
}

} // namespace ellindyer::app
