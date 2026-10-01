#include "Core/LogLevel.hpp"

#include <algorithm>
#include <cctype>
#include <string>


namespace ellindyer::core
{

std::string_view ToString(LogLevel level) noexcept
{
    switch (level)
    {
    case LogLevel::Trace:    return std::string_view{"trace"};
    case LogLevel::Debug:    return std::string_view{"debug"};
    case LogLevel::Info:     return std::string_view{"info"};
    case LogLevel::Warning:  return std::string_view{"warning"};
    case LogLevel::Error:    return std::string_view{"error"};
    case LogLevel::Critical: return std::string_view{"critical"};
    case LogLevel::Off:      return std::string_view{"off"};
    }
    return std::string_view{"info"};
}

LogLevel LogLevelFromString(std::string_view text) noexcept
{
    std::string lowered;
    lowered.reserve(text.size());
    for (char c : text)
    {
        lowered.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    if (lowered == "trace")    return LogLevel::Trace;
    if (lowered == "debug")    return LogLevel::Debug;
    if (lowered == "info")     return LogLevel::Info;
    if (lowered == "warning")  return LogLevel::Warning;
    if (lowered == "warn")     return LogLevel::Warning;
    if (lowered == "error")    return LogLevel::Error;
    if (lowered == "critical") return LogLevel::Critical;
    if (lowered == "off")      return LogLevel::Off;

    return LogLevel::Info;
}

bool IsValidLogLevel(LogLevel level) noexcept
{
    switch (level)
    {
    case LogLevel::Trace:
    case LogLevel::Debug:
    case LogLevel::Info:
    case LogLevel::Warning:
    case LogLevel::Error:
    case LogLevel::Critical:
    case LogLevel::Off:
        return true;
    }
    return false;
}

} // namespace ellindyer::core
