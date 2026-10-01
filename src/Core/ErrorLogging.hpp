#pragma once

#include <string_view>

#include "Core/Error.hpp"

namespace ellindyer::core
{

class ErrorLogging
{
public:
    ErrorLogging() = delete;

    static void LogError(const Error& error);

    static void LogError(std::string_view context, const Error& error);

    static void LogWarning(std::string_view context, const Error& error);
};

} // namespace ellindyer::core
