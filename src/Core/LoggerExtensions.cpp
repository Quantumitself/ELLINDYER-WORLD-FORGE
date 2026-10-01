#include "Core/LoggerExtensions.hpp"

namespace ellindyer::core
{

void LoggerExtensions::LogErrorObject(Logger& logger, const Error& error)
{
    if (error.IsSuccess())
    {
        return;
    }
    logger.LogError(error.ToDiagnosticString());
}

void LoggerExtensions::LogErrorObject(Logger& logger,
                                      std::string_view context,
                                      const Error& error)
{
    if (error.IsSuccess())
    {
        return;
    }

    std::string combined;
    combined.reserve(context.size() + 3 + error.Message().size());
    combined.append(context);
    combined.append(": ");
    combined.append(error.ToDiagnosticString());

    logger.LogError(combined);
}

void LoggerExtensions::LogSection(Logger& logger, std::string_view title)
{
    std::string combined;
    combined.reserve(title.size() + 8);
    combined.append("=== ");
    combined.append(title);
    combined.append(" ===");
    logger.LogInfo(combined);
}

void LoggerExtensions::LogKeyValue(Logger& logger,
                                   LogLevel level,
                                   std::string_view key,
                                   std::string_view value)
{
    std::string combined;
    combined.reserve(key.size() + 2 + value.size());
    combined.append(key);
    combined.append(": ");
    combined.append(value);
    logger.Log(level, combined);
}

} // namespace ellindyer::core
