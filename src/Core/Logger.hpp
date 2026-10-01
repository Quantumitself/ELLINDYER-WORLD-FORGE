#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

#include "Core/LogLevel.hpp"
#include "Core/LogSink.hpp"

namespace ellindyer::core
{

class Logger
{
public:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) noexcept = delete;
    Logger& operator=(Logger&&) noexcept = delete;

    void Initialize(std::string_view logger_name, LogLevel level);

    void Shutdown();

    [[nodiscard]] bool IsInitialized() const noexcept;

    void SetLevel(LogLevel level);

    [[nodiscard]] LogLevel GetLevel() const noexcept;

    void AddConsoleSink(const ConsoleSinkOptions& options);

    void AddFileSink(const FileSinkOptions& options);

    void AddRotatingFileSink(const RotatingFileSinkOptions& options);

    void RemoveAllSinks();

    void Flush();

    void Log(LogLevel level, std::string_view message);

    void LogTrace(std::string_view message);

    void LogDebug(std::string_view message);

    void LogInfo(std::string_view message);

    void LogWarning(std::string_view message);

    void LogError(std::string_view message);

    void LogCritical(std::string_view message);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

[[nodiscard]] Logger& GetLogger() noexcept;

} // namespace ellindyer::core
