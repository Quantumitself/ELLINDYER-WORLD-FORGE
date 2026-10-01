#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace ellindyer::core
{

class PathHelpers
{
public:
    PathHelpers() = delete;

    [[nodiscard]] static bool HasExtension(const std::filesystem::path& path,
                                           std::string_view extension);

    [[nodiscard]] static std::string GetExtensionLower(const std::filesystem::path& path);

    [[nodiscard]] static std::filesystem::path WithExtension(
        const std::filesystem::path& path,
        std::string_view extension);

    [[nodiscard]] static std::filesystem::path WithoutExtension(
        const std::filesystem::path& path);

    [[nodiscard]] static std::string GetFileNameWithoutExtension(
        const std::filesystem::path& path);

    [[nodiscard]] static std::filesystem::path SanitizeFileName(std::string_view name);

    [[nodiscard]] static bool IsWithinDirectory(const std::filesystem::path& path,
                                                const std::filesystem::path& directory);

    [[nodiscard]] static std::string JoinPathSegments(
        const std::vector<std::string>& segments);
};

} // namespace ellindyer::core
