#include <cstdio>
#include <cstdlib>

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
    std::printf("Build system initialized.%s", kLineEnding);
    std::fflush(stdout);
}

int RunApplication()
{
    PrintBanner();
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
