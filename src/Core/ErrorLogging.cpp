#include "Core/ErrorLogging.hpp"

#include <cstdio>

namespace ellindyer::core
{

namespace
{

#if defined(_WIN32)
constexpr const char* kLineEnding = "\r\n";
#else
constexpr const char* kLineEnding = "\n";
#endif

} // namespace

void ErrorLogging::LogError(const Error& error)
{
    if (error.IsSuccess())
    {
        return;
    }
    const std::string message = error.ToDiagnosticString();
    std::fprintf(stderr, "ERROR: %s%s", message.c_str(), kLineEnding);
    std::fflush(stderr);
}

void ErrorLogging::LogError(std::string_view context, const Error& error)
{
    if (error.IsSuccess())
    {
        return;
    }
    const std::string message = error.ToDiagnosticString();
    std::fprintf(stderr, "ERROR [%.*s]: %s%s",
                 static_cast<int>(context.size()),
                 context.data(),
                 message.c_str(),
                 kLineEnding);
    std::fflush(stderr);
}

void ErrorLogging::LogWarning(std::string_view context, const Error& error)
{
    const std::string message = error.ToDiagnosticString();
    std::fprintf(stderr, "WARNING [%.*s]: %s%s",
                 static_cast<int>(context.size()),
                 context.data(),
                 message.c_str(),
                 kLineEnding);
    std::fflush(stderr);
}

} // namespace ellindyer::core
