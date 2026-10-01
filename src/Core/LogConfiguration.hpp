#pragma once

#include <filesystem>

#include "Core/LogLevel.hpp"
#include "Core/LogSink.hpp"

namespace ellindyer::core
{

struct LogConfiguration
{
    LogLevel                level                    = LogLevel::Info;
    bool                    enable_console           = true;
    bool                    console_use_colors       = true;
    bool                    console_use_stderr       = false;
    bool                    enable_file              = true;
    bool                    file_rotate              = true;
    std::filesystem::path   file_path                = {};
    std::size_t             file_max_size_bytes      = 4U * 1024U * 1024U;
    std::size_t             file_max_count           = 4;
};

class LogConfigurationBuilder
{
public:
    LogConfigurationBuilder& WithLevel(LogLevel level) noexcept;

    LogConfigurationBuilder& WithConsole(bool enabled,
                                         bool use_colors = true,
                                         bool use_stderr = false) noexcept;

    LogConfigurationBuilder& WithFile(const std::filesystem::path& path,
                                      bool rotate = true,
                                      std::size_t max_size_bytes = 4U * 1024U * 1024U,
                                      std::size_t max_count = 4);

    LogConfigurationBuilder& WithDefaultLogFilePath(const std::filesystem::path& logs_directory,
                                                    const std::filesystem::path& file_name);

    [[nodiscard]] const LogConfiguration& Build() const noexcept;

private:
    LogConfiguration configuration_;
};

} // namespace ellindyer::core
