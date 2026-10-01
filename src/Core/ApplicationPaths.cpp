#include "Core/ApplicationPaths.hpp"

#include <system_error>

#if defined(_WIN32)
#    include <windows.h>
#    include <shlobj.h>
#endif

namespace ellindyer::core
{

namespace
{

#if defined(_WIN32)

std::filesystem::path GetKnownFolder(REFKNOWNFOLDERID folder_id)
{
    PWSTR raw_path = nullptr;
    const HRESULT result = SHGetKnownFolderPath(folder_id, 0, nullptr, &raw_path);
    if (FAILED(result) || raw_path == nullptr)
    {
        if (raw_path != nullptr)
        {
            CoTaskMemFree(raw_path);
        }
        return {};
    }
    std::filesystem::path path(raw_path);
    CoTaskMemFree(raw_path);
    return path;
}

#endif

} // namespace

std::filesystem::path ApplicationPaths::GetExecutablePath()
{
#if defined(_WIN32)
    wchar_t buffer[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (length == 0 || length == MAX_PATH)
    {
        return {};
    }
    return std::filesystem::path(buffer);
#else
    std::error_code ec;
    const std::filesystem::path exe = std::filesystem::read_symlink("/proc/self/exe", ec);
    if (ec)
    {
        return {};
    }
    return exe;
#endif
}

std::filesystem::path ApplicationPaths::GetExecutableDirectory()
{
    const std::filesystem::path exe = GetExecutablePath();
    if (exe.empty())
    {
        return {};
    }
    return exe.parent_path();
}

std::filesystem::path ApplicationPaths::GetResourcesDirectory()
{
    const std::filesystem::path dir = GetExecutableDirectory();
    if (dir.empty())
    {
        return {};
    }
    return dir / "resources";
}

std::filesystem::path ApplicationPaths::GetIconsDirectory()
{
    return GetResourcesDirectory() / "icons";
}

std::filesystem::path ApplicationPaths::GetFontsDirectory()
{
    return GetResourcesDirectory() / "fonts";
}

std::filesystem::path ApplicationPaths::GetThemesDirectory()
{
    return GetResourcesDirectory() / "themes";
}

std::filesystem::path ApplicationPaths::GetUserDataDirectory()
{
#if defined(_WIN32)
    std::filesystem::path base = GetKnownFolder(FOLDERID_LocalAppData);
    if (base.empty())
    {
        return {};
    }
    return base / "EllindyerWorldForge";
#else
    const char* home = std::getenv("HOME");
    if (home == nullptr)
    {
        return {};
    }
    return std::filesystem::path(home) / ".ellindyer-world-forge";
#endif
}

std::filesystem::path ApplicationPaths::GetLogsDirectory()
{
    return GetUserDataDirectory() / "logs";
}

std::filesystem::path ApplicationPaths::GetConfigurationDirectory()
{
    return GetUserDataDirectory() / "config";
}

bool ApplicationPaths::EnsureUserDirectoriesExist()
{
    const std::filesystem::path user_dir = GetUserDataDirectory();
    if (user_dir.empty())
    {
        return false;
    }

    std::error_code ec;
    std::filesystem::create_directories(user_dir, ec);
    if (ec)
    {
        return false;
    }

    std::filesystem::create_directories(GetLogsDirectory(), ec);
    if (ec)
    {
        return false;
    }

    std::filesystem::create_directories(GetConfigurationDirectory(), ec);
    if (ec)
    {
        return false;
    }

    return true;
}

} // namespace ellindyer::core
