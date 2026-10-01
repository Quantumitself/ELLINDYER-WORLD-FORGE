#pragma once

#include "Core/LogConfiguration.hpp"
#include "Core/Logger.hpp"

namespace ellindyer::core
{

class LogBootstrap
{
public:
    LogBootstrap() = delete;

    static bool Initialize(const LogConfiguration& configuration);

    static void Shutdown();
};

} // namespace ellindyer::core
