#include "Core/FileWatcher.hpp"

#include <system_error>
#include <utility>

#include "Core/FileHelpers.hpp"

namespace ellindyer::core
{

FileWatcher::FileWatcher() = default;

FileWatcher::~FileWatcher() = default;

FileWatcher::FileWatcher(FileWatcher&&) noexcept = default;

FileWatcher& FileWatcher::operator=(FileWatcher&&) noexcept = default;

Result<void> FileWatcher::Track(const std::filesystem::path& path)
{
    if (path.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument, "Watcher path is empty."));
    }

    TrackedEntry entry{};
    entry.path = path;
    entry.existed = FileHelpers::Exists(path);

    if (entry.existed)
    {
        const Result<std::filesystem::file_time_type> write_time =
            FileHelpers::GetLastWriteTime(path);
        if (write_time.HasError())
        {
            return Result<void>(write_time.GetError());
        }
        entry.last_write_time = write_time.Value();

        const Result<std::uintmax_t> size = FileHelpers::GetFileSize(path);
        entry.last_size = size.HasValue() ? size.Value() : 0;
    }
    else
    {
        entry.last_write_time = std::filesystem::file_time_type{};
        entry.last_size = 0;
    }

    tracked_.push_back(std::move(entry));
    return Result<void>{};
}

void FileWatcher::Clear()
{
    tracked_.clear();
}

std::vector<FileChangeEvent> FileWatcher::Poll()
{
    std::vector<FileChangeEvent> events;

    for (TrackedEntry& entry : tracked_)
    {
        const bool now_exists = FileHelpers::Exists(entry.path);

        if (!entry.existed && now_exists)
        {
            FileChangeEvent event{};
            event.path = entry.path;
            event.kind = FileChangeKind::Created;

            const Result<std::filesystem::file_time_type> write_time =
                FileHelpers::GetLastWriteTime(entry.path);
            if (write_time.HasValue())
            {
                event.timestamp = write_time.Value();
                entry.last_write_time = write_time.Value();
            }

            const Result<std::uintmax_t> size = FileHelpers::GetFileSize(entry.path);
            entry.last_size = size.HasValue() ? size.Value() : 0;
            entry.existed = true;

            events.push_back(std::move(event));
            continue;
        }

        if (entry.existed && !now_exists)
        {
            FileChangeEvent event{};
            event.path = entry.path;
            event.kind = FileChangeKind::Removed;
            event.timestamp = std::filesystem::file_time_type{};

            entry.existed = false;
            entry.last_write_time = std::filesystem::file_time_type{};
            entry.last_size = 0;

            events.push_back(std::move(event));
            continue;
        }

        if (entry.existed && now_exists)
        {
            const Result<std::filesystem::file_time_type> write_time =
                FileHelpers::GetLastWriteTime(entry.path);
            const Result<std::uintmax_t> size = FileHelpers::GetFileSize(entry.path);

            const std::filesystem::file_time_type new_time = write_time.HasValue()
                ? write_time.Value()
                : entry.last_write_time;
            const std::uintmax_t new_size = size.HasValue() ? size.Value() : entry.last_size;

            if (new_time != entry.last_write_time || new_size != entry.last_size)
            {
                FileChangeEvent event{};
                event.path = entry.path;
                event.kind = FileChangeKind::Modified;
                event.timestamp = new_time;

                entry.last_write_time = new_time;
                entry.last_size = new_size;

                events.push_back(std::move(event));
            }
        }
    }

    return events;
}

} // namespace ellindyer::core
