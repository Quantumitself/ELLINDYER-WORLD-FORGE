#include "Core/Logger.hpp"

#include <mutex>
#include <utility>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

namespace ellindyer::core
{

namespace
{

[[nodiscard]] spdlog::level::level_enum ToSpdlogLevel(LogLevel level) noexcept
{
    switch (level)
    {
    case LogLevel::Trace:    return spdlog::level::trace;
    case LogLevel::Debug:    return spdlog::level::debug;
    case LogLevel::Info:     return spdlog::level::info;
    case LogLevel::Warning:  return spdlog::level::warn;
    case LogLevel::Error:    return spdlog::level::err;
    case LogLevel::Critical: return spdlog::level::critical;
    case LogLevel::Off:      return spdlog::level::off;
    }
    return spdlog::level::info;
}

[[nodiscard]] std::filesystem::path EnsureParentDirectory(const std::filesystem::path& path)
{
    const std::filesystem::path parent = path.parent_path();
    if (!parent.empty())
    {
        std::error_code ec;
        std::filesystem::create_directories(parent, ec);
    }
    return path;
}

} // namespace

struct Logger::Impl
{
    std::mutex                                        mutex;
    std::shared_ptr<spdlog::logger>                   logger;
    std::vector<spdlog::sink_ptr>                     sinks;
    std::string                                       name;
    LogLevel                                          level = LogLevel::Info;
    bool                                              initialized = false;
};

Logger::Logger()
    : impl_(std::make_unique<Impl>())
{
}

Logger::~Logger()
{
    Shutdown();
}

void Logger::Initialize(std::string_view logger_name, LogLevel level)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);

    if (impl_->initialized)
    {
        return;
    }

    impl_->name  = std::string(logger_name);
    impl_->level = level;

    if (impl_->name.empty())
    {
        impl_->name = "EllindyerWorldForge";
    }

    impl_->logger = std::make_shared<spdlog::logger>(impl_->name);
    impl_->logger->set_level(ToSpdlogLevel(level));
    impl_->logger->flush_on(spdlog::level::warn);
    impl_->logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");

    spdlog::register_logger(impl_->logger);

    impl_->initialized = true;
}

void Logger::Shutdown()
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->initialized)
    {
        return;
    }

    if (impl_->logger)
    {
        impl_->logger->flush();
        spdlog::drop(impl_->name);
        impl_->logger.reset();
    }

    impl_->sinks.clear();
    impl_->initialized = false;
    impl_->level = LogLevel::Info;
    impl_->name.clear();
}

bool Logger::IsInitialized() const noexcept
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->initialized;
}

void Logger::SetLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->level = level;
    if (impl_->logger)
    {
        impl_->logger->set_level(ToSpdlogLevel(level));
    }
}

LogLevel Logger::GetLevel() const noexcept
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->level;
}

void Logger::AddConsoleSink(const ConsoleSinkOptions& options)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->initialized)
    {
        return;
    }

    spdlog::sink_ptr sink;
    if (options.use_colors)
    {
        if (options.use_stderr)
        {
            auto color_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
            color_sink->set_level(ToSpdlogLevel(options.level));
            color_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
            sink = color_sink;
        }
        else
        {
            auto color_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            color_sink->set_level(ToSpdlogLevel(options.level));
            color_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
            sink = color_sink;
        }
    }
    else
    {
        if (options.use_stderr)
        {
            auto plain_sink = std::make_shared<spdlog::sinks::stderr_sink_mt>();
            plain_sink->set_level(ToSpdlogLevel(options.level));
            plain_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
            sink = plain_sink;
        }
        else
        {
            auto plain_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
            plain_sink->set_level(ToSpdlogLevel(options.level));
            plain_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
            sink = plain_sink;
        }
    }

    impl_->sinks.push_back(std::move(sink));
    impl_->logger->sinks() = impl_->sinks;
}

void Logger::AddFileSink(const FileSinkOptions& options)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->initialized)
    {
        return;
    }

    if (options.path.empty())
    {
        return;
    }

    const std::filesystem::path full_path = EnsureParentDirectory(options.path);

    if (!options.rotate)
    {
        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            full_path.string(), options.truncate);
        sink->set_level(ToSpdlogLevel(options.level));
        sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");
        impl_->sinks.push_back(std::move(sink));
    }
    else
    {
        auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            full_path.string(), options.max_size_bytes, options.max_files);
        sink->set_level(ToSpdlogLevel(options.level));
        sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");
        impl_->sinks.push_back(std::move(sink));
    }

    impl_->logger->sinks() = impl_->sinks;
}

void Logger::AddRotatingFileSink(const RotatingFileSinkOptions& options)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->initialized)
    {
        return;
    }

    if (options.base_path.empty())
    {
        return;
    }

    const std::filesystem::path full_path = EnsureParentDirectory(options.base_path);

    auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        full_path.string(), options.max_size_bytes, options.max_files);
    sink->set_level(ToSpdlogLevel(options.level));
    sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");
    impl_->sinks.push_back(std::move(sink));
    impl_->logger->sinks() = impl_->sinks;
}

void Logger::RemoveAllSinks()
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (!impl_->initialized)
    {
        return;
    }
    impl_->sinks.clear();
    impl_->logger->sinks().clear();
}

void Logger::Flush()
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->logger)
    {
        impl_->logger->flush();
    }
}

void Logger::Log(LogLevel level, std::string_view message)
{
    std::shared_ptr<spdlog::logger> logger_snapshot;
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        if (!impl_->initialized || !impl_->logger)
        {
            return;
        }
        logger_snapshot = impl_->logger;
    }

    logger_snapshot->log(ToSpdlogLevel(level), "{}", message);
}

void Logger::LogTrace(std::string_view message)    { Log(LogLevel::Trace, message); }
void Logger::LogDebug(std::string_view message)    { Log(LogLevel::Debug, message); }
void Logger::LogInfo(std::string_view message)     { Log(LogLevel::Info, message); }
void Logger::LogWarning(std::string_view message)  { Log(LogLevel::Warning, message); }
void Logger::LogError(std::string_view message)    { Log(LogLevel::Error, message); }
void Logger::LogCritical(std::string_view message) { Log(LogLevel::Critical, message); }

Logger& GetLogger() noexcept
{
    static Logger instance;
    return instance;
}

} // namespace ellindyer::core
