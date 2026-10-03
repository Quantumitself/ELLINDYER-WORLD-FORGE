#pragma once

#include <filesystem>

namespace ellindyer::ui::dialogs
{

class PlatformFileDialog
{
public:
    PlatformFileDialog() = delete;

    // Returns an empty path if the user cancels or if the platform
    // does not support a native folder picker.
    [[nodiscard]] static std::filesystem::path PickFolder(
        const std::filesystem::path& initial_directory = std::filesystem::path{});
};

} // namespace ellindyer::ui::dialogs
