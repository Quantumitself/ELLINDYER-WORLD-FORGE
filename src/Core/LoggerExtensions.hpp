#pragma once

#include <string>
#include <string_view>

#include "Core/Error.hpp"
#include "Core/Logger.hpp"

namespace ellindyer::core
{

class LoggerExtensions
{
public:
    LoggerExtensions() = delete;

    static void LogErrorObject(Logger& logger, const Error& error);

    static void LogErrorObject(Logger& logger, std::string_view context, const Error& error);

    static void LogSection(Logger& logger, std::string_view title);

    static void LogKeyValue(Logger& logger,
                            LogLevel level,
                            std::string_view key,
                            std::string_view value);
};

} // namespace ellindyer::core
