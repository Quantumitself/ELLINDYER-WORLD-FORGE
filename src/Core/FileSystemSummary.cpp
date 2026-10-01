#include "Core/FileSystemSummary.hpp"

#include <iomanip>
#include <sstream>
#include <system_error>

namespace ellindyer::core
{

DirectorySummary FileSystemSummary::Summarize(const std::filesystem::path& directory,
                                              bool recursive)
{
    DirectorySummary summary{};
    summary.directory = directory;

    std::error_code ec;
    if (!std::filesystem::exists(directory, ec) || !std::filesystem::is_directory(directory, ec))
    {
        return summary;
    }

    const auto process_entry = [&summary](const std::filesystem::directory_entry& entry)
    {
        std::error_code inner_ec;
        if (entry.is_directory(inner_ec))
        {
            summary.directory_count += 1;
        }
        else if (entry.is_regular_file(inner_ec))
        {
            summary.file_count += 1;
            const std::uintmax_t size = entry.file_size(inner_ec);
            if (!inner_ec)
            {
                summary.total_bytes += size;
            }
        }
    };

    if (recursive)
    {
        std::filesystem::recursive_directory_iterator it(directory, ec);
        if (ec)
        {
            return summary;
        }
        for (const auto& entry : it)
        {
            process_entry(entry);
        }
    }
    else
    {
        std::filesystem::directory_iterator it(directory, ec);
        if (ec)
        {
            return summary;
        }
        for (const auto& entry : it)
        {
            process_entry(entry);
        }
    }

    return summary;
}

std::string FileSystemSummary::FormatBytes(std::uintmax_t bytes)
{
    constexpr const char* kUnits[] = {"B", "KiB", "MiB", "GiB", "TiB", "PiB"};

    double value = static_cast<double>(bytes);
    std::size_t unit_index = 0;

    while (value >= 1024.0 && unit_index < (sizeof(kUnits) / sizeof(kUnits[0])) - 1)
    {
        value /= 1024.0;
        ++unit_index;
    }

    std::ostringstream stream;
    if (unit_index == 0)
    {
        stream << bytes << ' ' << kUnits[unit_index];
    }
    else
    {
        stream << std::fixed << std::setprecision(2) << value << ' ' << kUnits[unit_index];
    }
    return stream.str();
}

std::string FileSystemSummary::FormatSummary(const DirectorySummary& summary)
{
    std::ostringstream stream;
    stream << "Directory: " << summary.directory.generic_string() << '\n';
    stream << "Files: " << summary.file_count << '\n';
    stream << "Subdirectories: " << summary.directory_count << '\n';
    stream << "Total size: " << FormatBytes(summary.total_bytes);
    return stream.str();
}

} // namespace ellindyer::core
