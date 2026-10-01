#include <cstdio>
#include <cstdlib>

#include "ThirdPartyProbe.hpp"

#if defined(_WIN32)
#include <windows.h>
#endif

namespace
{

#if defined(_WIN32)
constexpr const char* kLineEnding = "\r\n";
#else
constexpr const char* kLineEnding = "\n";
#endif

constexpr int kVersionMajor = ELLINDYER_WORLD_FORGE_VERSION_MAJOR;
constexpr int kVersionMinor = ELLINDYER_WORLD_FORGE_VERSION_MINOR;
constexpr int kVersionPatch = ELLINDYER_WORLD_FORGE_VERSION_PATCH;

void PrintBanner()
{
    std::printf("Ellindyer World Forge %d.%d.%d%s",
                kVersionMajor,
                kVersionMinor,
                kVersionPatch,
                kLineEnding);
    std::printf("World & Game Design Studio%s", kLineEnding);
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

int RunApplication()
{
    PrintBanner();
    PrintThirdPartyStatus();
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
