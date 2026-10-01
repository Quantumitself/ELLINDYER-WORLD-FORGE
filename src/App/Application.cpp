#include "App/Application.hpp"

#include <cstdio>
#include <string>
#include <vector>

#include "Core/Error.hpp"
#include "Core/LogMacros.hpp"

namespace ellindyer::app
{

namespace
{

#if defined(_WIN32)
constexpr const char* kLineEnding = "\r\n";
#else
constexpr const char* kLineEnding = "\n";
#endif

std::vector<std::string> CollectArguments(int argc, char** argv)
{
    std::vector<std::string> arguments;
    if (argc <= 1 || argv == nullptr)
    {
        return arguments;
    }
    arguments.reserve(static_cast<std::size_t>(argc - 1));
    for (int index = 1; index < argc; ++index)
    {
        if (argv[index] != nullptr)
        {
            arguments.emplace_back(argv[index]);
        }
    }
    return arguments;
}

void PrintStartupFailure(const ellindyer::core::Error& error)
{
    std::fprintf(stderr,
                 "Ellindyer World Forge failed to start: %s%s",
                 error.ToDiagnosticString().c_str(),
                 kLineEnding);
    std::fflush(stderr);
}

} // namespace

Application::Application() = default;

Application::~Application() = default;

int Application::Execute(int argc, char** argv)
{
    const std::vector<std::string> arguments = CollectArguments(argc, argv);
    (void)arguments;

    const ellindyer::core::Result<void> init_result = lifecycle_.Initialize();
    if (init_result.HasError())
    {
        PrintStartupFailure(init_result.GetError());
        lifecycle_.Shutdown();
        return 1;
    }

    const ellindyer::core::Result<int> run_result = lifecycle_.Run();
    if (run_result.HasError())
    {
        ELLINDYER_LOG_ERROR(run_result.GetError().ToDiagnosticString());
        lifecycle_.Shutdown();
        return 1;
    }

    const int exit_code = run_result.Value();
    lifecycle_.Shutdown();
    return exit_code;
}

} // namespace ellindyer::app
