#include "Core/LogConfiguration.hpp"

namespace ellindyer::core
{

LogConfigurationBuilder& LogConfigurationBuilder::WithLevel(LogLevel level) noexcept
{
    configuration_.level = level;
    return *this;
}

LogConfigurationBuilder& LogConfigurationBuilder::WithConsole(bool enabled,
                                                              bool use_colors,
                                                              bool use_stderr) noexcept
{
    configuration_.enable_console     = enabled;
    configuration_.console_use_colors = use_colors;
    configuration_.console_use_stderr = use_stderr;
    return *this;
}

LogConfigurationBuilder& LogConfigurationBuilder::WithFile(const std::filesystem::path& path,
                                                           bool rotate,
                                                           std::size_t max_size_bytes,
                                                           std::size_t max_count)
{
    configuration_.enable_file           = !path.empty();
    configuration_.file_path             = path;
    configuration_.file_rotate           = rotate;
    configuration_.file_max_size_bytes   = max_size_bytes;
    configuration_.file_max_count        = max_count;
    return *this;
}

LogConfigurationBuilder& LogConfigurationBuilder::WithDefaultLogFilePath(
    const std::filesystem::path& logs_directory,
    const std::filesystem::path& file_name)
{
    if (logs_directory.empty() || file_name.empty())
    {
        configuration_.enable_file = false;
        configuration_.file_path.clear();
        return *this;
    }

    configuration_.enable_file = true;
    configuration_.file_path   = logs_directory / file_name;
    return *this;
}

const LogConfiguration& LogConfigurationBuilder::Build() const noexcept
{
    return configuration_;
}

} // namespace ellindyer::core
