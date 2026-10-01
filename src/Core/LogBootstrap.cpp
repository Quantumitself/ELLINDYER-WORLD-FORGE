#include "Core/LogBootstrap.hpp"

#include <string>

namespace ellindyer::core
{

bool LogBootstrap::Initialize(const LogConfiguration& configuration)
{
    Logger& logger = GetLogger();
    if (logger.IsInitialized())
    {
        return true;
    }

    logger.Initialize("EllindyerWorldForge", configuration.level);

    if (configuration.enable_console)
    {
        ConsoleSinkOptions console{};
        console.use_colors = configuration.console_use_colors;
        console.use_stderr = configuration.console_use_stderr;
        console.level      = configuration.level;
        logger.AddConsoleSink(console);
    }

    if (configuration.enable_file && !configuration.file_path.empty())
    {
        FileSinkOptions file{};
        file.path           = configuration.file_path;
        file.rotate         = configuration.file_rotate;
        file.max_size_bytes = configuration.file_max_size_bytes;
        file.max_files      = configuration.file_max_count;
        file.truncate       = false;
        file.level          = configuration.level;
        logger.AddFileSink(file);
    }

    return true;
}

void LogBootstrap::Shutdown()
{
    Logger& logger = GetLogger();
    if (!logger.IsInitialized())
    {
        return;
    }
    logger.Flush();
    logger.Shutdown();
}

} // namespace ellindyer::core
