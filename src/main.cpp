#include <cstdlib>
#include <string>

#include "App/Application.hpp"

#if defined(_WIN32)
#    include <windows.h>
#endif

#if defined(_WIN32)

namespace
{

std::string WideToNarrow(const wchar_t* wide)
{
    if (wide == nullptr)
    {
        return std::string{};
    }

    const int required = WideCharToMultiByte(CP_UTF8,
                                             0,
                                             wide,
                                             -1,
                                             nullptr,
                                             0,
                                             nullptr,
                                             nullptr);
    if (required <= 0)
    {
        return std::string{};
    }

    std::string result;
    result.resize(static_cast<std::size_t>(required - 1));
    WideCharToMultiByte(CP_UTF8,
                        0,
                        wide,
                        -1,
                        result.data(),
                        required,
                        nullptr,
                        nullptr);
    return result;
}

} // namespace

int WINAPI wWinMain(HINSTANCE /*hInstance*/,
                    HINSTANCE /*hPrevInstance*/,
                    PWSTR     /*pCmdLine*/,
                    int       /*nCmdShow*/)
{
    ellindyer::app::Application application;
    return application.Execute(0, nullptr);
}

#else

int main(int argc, char** argv)
{
    ellindyer::app::Application application;
    return application.Execute(argc, argv);
}

#endif
