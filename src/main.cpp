#include <cstdio>
#include <cstdlib>
#include <string>

#include "Core/ApplicationInfo.hpp"
#include "Core/ApplicationPaths.hpp"
#include "Core/BuildInfo.hpp"
#include "Core/Identifier.hpp"
#include "Core/IdentifierFormatter.hpp"
#include "Core/IdentifierGenerator.hpp"
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

int RunApplication()
{
    PrintApplicationBanner();
    PrintBuildInformation();
    PrintPathInformation();
    PrintThirdPartyStatus();
    PrintIdentifierInformation();
    std::fflush(stdout);
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
