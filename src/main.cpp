#include <cstdio>
#include <cstdlib>
#include <string>

#include "Core/ApplicationInfo.hpp"
#include "Core/ApplicationPaths.hpp"
#include "Core/BuildInfo.hpp"
#include "Core/Error.hpp"
#include "Core/ErrorCode.hpp"
#include "Core/ErrorLogging.hpp"
#include "Core/Identifier.hpp"
#include "Core/IdentifierFormatter.hpp"
#include "Core/IdentifierGenerator.hpp"
#include "Core/Result.hpp"
#include "Core/ResultHelpers.hpp"
#include "Core/Version.hpp"
#include "ThirdPartyProbe.hpp"

#if defined(_WIN32)
#    include <windows.h>
#endif

namespace
{

#if defined(_WIN32)
constexpr const char* kLineEnding = "\r\n";
#else
constexpr const char* kLineEnding = "\n";
#endif

void PrintApplicationBanner()
{
    const ellindyer::core::ApplicationInfo info = ellindyer::core::GetApplicationInfo();
    std::printf("%s %s%s",
                info.name.c_str(),
                info.version.c_str(),
                kLineEnding);
    std::printf("%s%s", info.tagline.c_str(), kLineEnding);
    std::printf("Organization: %s%s", info.organization.c_str(), kLineEnding);
}

void PrintBuildInformation()
{
    const ellindyer::core::BuildInfo info = ellindyer::core::GetBuildInfo();
    std::printf("Build configuration : %s%s",
                std::string(ellindyer::core::GetBuildConfigurationName(info.configuration)).c_str(),
                kLineEnding);
    std::printf("Compiler            : %.*s %.*s%s",
                static_cast<int>(info.compiler_name.size()),
                info.compiler_name.data(),
                static_cast<int>(info.compiler_version.size()),
                info.compiler_version.data(),
                kLineEnding);
    std::printf("Platform            : %.*s %.*s%s",
                static_cast<int>(info.platform.size()),
                info.platform.data(),
                static_cast<int>(info.architecture.size()),
                info.architecture.data(),
                kLineEnding);
    std::printf("C++ standard        : %.*s%s",
                static_cast<int>(info.cpp_standard.size()),
                info.cpp_standard.data(),
                kLineEnding);
    std::printf("Build date          : %.*s %.*s%s",
                static_cast<int>(info.build_date.size()),
                info.build_date.data(),
                static_cast<int>(info.build_time.size()),
                info.build_time.data(),
                kLineEnding);
}

void PrintPathInformation()
{
    const std::filesystem::path exe_dir = ellindyer::core::ApplicationPaths::GetExecutableDirectory();
    const std::filesystem::path res_dir = ellindyer::core::ApplicationPaths::GetResourcesDirectory();
    const std::filesystem::path usr_dir = ellindyer::core::ApplicationPaths::GetUserDataDirectory();

    std::printf("Executable directory: %s%s",
                exe_dir.string().c_str(),
                kLineEnding);
    std::printf("Resources directory : %s%s",
                res_dir.string().c_str(),
                kLineEnding);
    std::printf("User data directory : %s%s",
                usr_dir.string().c_str(),
                kLineEnding);

    const bool user_dirs_ready = ellindyer::core::ApplicationPaths::EnsureUserDirectoriesExist();
    std::printf("User directories    : %s%s",
                user_dirs_ready ? "ready" : "unavailable",
                kLineEnding);
}

void PrintThirdPartyStatus()
{
    const ellindyer::third_party::LibraryStatus status =
        ellindyer::third_party::QueryLibraryStatus();

    std::printf("Third-party libraries:%s", kLineEnding);
    std::printf("  nlohmann/json : %s%s",
                status.json_available ? "available" : "not present",
                kLineEnding);
    std::printf("  spdlog        : %s%s",
                status.spdlog_available ? "available" : "not present",
                kLineEnding);
    std::printf("  stb           : %s%s",
                status.stb_available ? "available" : "not present",
                kLineEnding);
    std::printf("  Dear ImGui    : %s%s",
                status.imgui_available ? "available" : "not present",
                kLineEnding);
}

void PrintIdentifierInformation()
{
    using ellindyer::core::Identifier;
    using ellindyer::core::IdentifierFormatter;
    using ellindyer::core::IdentifierGenerator;

    const Identifier generated = IdentifierGenerator::NewIdentifier();
    const Identifier deterministic = IdentifierGenerator::DeterministicIdentifier(0x9e3779b97f4a7c15ULL);
    const Identifier from_text = ellindyer::core::MakeIdentifier("NPC-000001");
    const Identifier nil = Identifier::Nil();

    std::printf("Identifier examples:%s", kLineEnding);
    std::printf("  generated    : %s (short: %s)%s",
                IdentifierFormatter::FormatFull(generated).c_str(),
                IdentifierFormatter::FormatShort(generated).c_str(),
                kLineEnding);
    std::printf("  deterministic: %s (short: %s)%s",
                IdentifierFormatter::FormatFull(deterministic).c_str(),
                IdentifierFormatter::FormatShort(deterministic).c_str(),
                kLineEnding);
    std::printf("  from text    : %s (short: %s)%s",
                IdentifierFormatter::FormatFull(from_text).c_str(),
                IdentifierFormatter::FormatShort(from_text).c_str(),
                kLineEnding);
    std::printf("  nil          : %s (is_nil: %s)%s",
                IdentifierFormatter::FormatFull(nil).c_str(),
                nil.IsNil() ? "true" : "false",
                kLineEnding);
    std::printf("  prefixed     : %s%s",
                IdentifierFormatter::FormatPrefixed("NPC", from_text).c_str(),
                kLineEnding);
    std::printf("  prefixed(sh) : %s%s",
                IdentifierFormatter::FormatPrefixedShort("NPC", from_text).c_str(),
                kLineEnding);

    Identifier parsed{};
    const bool parsed_ok = Identifier::TryParse(from_text.ToString(), parsed);
    std::printf("  round-trip   : %s%s",
                (parsed_ok && parsed == from_text) ? "ok" : "failed",
                kLineEnding);
}

ellindyer::core::Result<int> ParsePositiveInteger(const std::string& text)
{
    using ellindyer::core::Error;
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::MakeResult;
    using ellindyer::core::Result;

    if (text.empty())
    {
        return Result<int>(MakeError(ErrorCode::InvalidArgument, "text is empty"));
    }

    int value = 0;
    for (char c : text)
    {
        if (c < '0' || c > '9')
        {
            return Result<int>(MakeError(ErrorCode::ParseError, "non-digit character encountered"));
        }
        value = value * 10 + (c - '0');
    }

    if (value <= 0)
    {
        return Result<int>(MakeError(ErrorCode::OutOfRange, "value must be positive"));
    }

    return MakeResult(value);
}

void PrintErrorInformation()
{
    using ellindyer::core::Error;
    using ellindyer::core::ErrorCode;
    using ellindyer::core::ErrorLogging;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    const Error success = Error::Success();
    const Error failure = MakeError(ErrorCode::FileNotFound, "Elyndra.png");
    const Error generic = Error::FromCode(ErrorCode::InvalidSchema);

    std::printf("Error examples:%s", kLineEnding);
    std::printf("  success     : is_success=%s is_failure=%s%s",
                success.IsSuccess() ? "true" : "false",
                success.IsFailure() ? "true" : "false",
                kLineEnding);
    std::printf("  failure     : %s%s",
                failure.ToDiagnosticString().c_str(),
                kLineEnding);
    std::printf("  generic     : %s%s",
                generic.ToDiagnosticString().c_str(),
                kLineEnding);

    const Result<int> ok_result = ParsePositiveInteger("42");
    const Result<int> empty_result = ParsePositiveInteger("");
    const Result<int> non_digit_result = ParsePositiveInteger("12x");
    const Result<int> zero_result = ParsePositiveInteger("0");

    std::printf("Result<int> examples:%s", kLineEnding);
    std::printf("  \"42\"  : %s (%s)%s",
                ok_result.HasValue() ? "value" : "error",
                ok_result.HasValue() ? std::to_string(ok_result.Value()).c_str()
                                     : ok_result.GetError().ToDiagnosticString().c_str(),
                kLineEnding);
    std::printf("  \"\"    : %s (%s)%s",
                empty_result.HasValue() ? "value" : "error",
                empty_result.HasValue() ? std::to_string(empty_result.Value()).c_str()
                                        : empty_result.GetError().ToDiagnosticString().c_str(),
                kLineEnding);
    std::printf("  \"12x\" : %s (%s)%s",
                non_digit_result.HasValue() ? "value" : "error",
                non_digit_result.HasValue() ? std::to_string(non_digit_result.Value()).c_str()
                                            : non_digit_result.GetError().ToDiagnosticString().c_str(),
                kLineEnding);
    std::printf("  \"0\"   : %s (%s)%s",
                zero_result.HasValue() ? "value" : "error",
                zero_result.HasValue() ? std::to_string(zero_result.Value()).c_str()
                                       : zero_result.GetError().ToDiagnosticString().c_str(),
                kLineEnding);

    ErrorLogging::LogError("bootstrap", failure);
}

int RunApplication()
{
    PrintApplicationBanner();
    PrintBuildInformation();
    PrintPathInformation();
    PrintThirdPartyStatus();
    PrintIdentifierInformation();
    PrintErrorInformation();
    std::fflush(stdout);
    std::fflush(stderr);
    return EXIT_SUCCESS;
}

} // namespace

#if defined(_WIN32)

int WINAPI wWinMain(HINSTANCE /*hInstance*/,
                    HINSTANCE /*hPrevInstance*/,
                    PWSTR     /*pCmdLine*/,
                    int       /*nCmdShow*/)
{
    return RunApplication();
}

#else

int main(int /*argc*/, char** /*argv*/)
{
    return RunApplication();
}

#endif
