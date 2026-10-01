#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <vector>

#include "Core/Error.hpp"
#include "Core/Result.hpp"

namespace ellindyer::core
{

enum class FileChangeKind : std::uint8_t
{
    Created,
    Modified,
    Removed
};

struct FileChangeEvent
{
    std::filesystem::path path;
    FileChangeKind        kind;
    std::filesystem::file_time_type timestamp;
};

class FileWatcher
{
public:
    FileWatcher();
    ~FileWatcher();

    FileWatcher(const FileWatcher&) = delete;
    FileWatcher& operator=(const FileWatcher&) = delete;
    FileWatcher(FileWatcher&&) noexcept;
    FileWatcher& operator=(FileWatcher&&) noexcept;

    [[nodiscard]] Result<void> Track(const std::filesystem::path& path);

    void Clear();

    [[nodiscard]] std::vector<FileChangeEvent> Poll();

private:
    struct TrackedEntry
    {
        std::filesystem::path path;
        std::filesystem::file_time_type last_write_time;
        std::uintmax_t last_size;
        bool existed;
    };

    std::vector<TrackedEntry> tracked_;
};

} // namespace ellindyer::core
