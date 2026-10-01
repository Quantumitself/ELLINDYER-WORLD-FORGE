#pragma once

#include <cstdint>
#include <string_view>

namespace ellindyer::core
{

enum class LogLevel : std::uint8_t
{
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Off
};

[[nodiscard]] std::string_view ToString(LogLevel level) noexcept;

[[nodiscard]] LogLevel LogLevelFromString(std::string_view text) noexcept;

[[nodiscard]] bool IsValidLogLevel(LogLevel level) noexcept;

} // namespace ellindyer::core
