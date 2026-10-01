#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "Core/LogLevel.hpp"

namespace ellindyer::core
{

struct ConsoleSinkOptions
{
    bool  use_colors = true;
    bool  use_stderr = false;
    LogLevel level   = LogLevel::Trace;
};

struct FileSinkOptions
{
    std::filesystem::path path;
    bool                  truncate     = false;
    bool                  rotate       = true;
    std::size_t           max_size_bytes = 4U * 1024U * 1024U;
    std::size_t           max_files    = 4;
    LogLevel              level        = LogLevel::Trace;
};

struct RotatingFileSinkOptions
{
    std::filesystem::path base_path;
    std::size_t           max_size_bytes = 4U * 1024U * 1024U;
    std::size_t           max_files      = 4;
    LogLevel              level          = LogLevel::Trace;
};

} // namespace ellindyer::core
