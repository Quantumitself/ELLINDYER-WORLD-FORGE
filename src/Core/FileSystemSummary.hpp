#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace ellindyer::core
{

struct FileSystemEntryInfo
{
    std::filesystem::path path;
    bool is_directory = false;
    bool is_regular_file = false;
    std::uintmax_t size = 0;
};

struct DirectorySummary
{
    std::filesystem::path directory;
    std::uint64_t file_count = 0;
    std::uint64_t directory_count = 0;
    std::uintmax_t total_bytes = 0;
};

class FileSystemSummary
{
public:
    FileSystemSummary() = delete;

    [[nodiscard]] static DirectorySummary Summarize(const std::filesystem::path& directory,
                                                    bool recursive = true);

    [[nodiscard]] static std::string FormatBytes(std::uintmax_t bytes);

    [[nodiscard]] static std::string FormatSummary(const DirectorySummary& summary);
};

} // namespace ellindyer::core
