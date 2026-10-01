#include "Core/BuildInfo.hpp"

#include <sstream>

namespace ellindyer::core
{

namespace
{

constexpr BuildConfiguration DetectBuildConfiguration() noexcept
{
#if defined(ELLINDYER_BUILD_DEBUG)
    return BuildConfiguration::Debug;
#elif defined(ELLINDYER_BUILD_RELEASE)
    return BuildConfiguration::Release;
#elif defined(ELLINDYER_BUILD_RELWITHDEBINFO)
    return BuildConfiguration::RelWithDebInfo;
#elif defined(ELLINDYER_BUILD_MINSIZEREL)
    return BuildConfiguration::MinSizeRel;
#elif defined(NDEBUG)
    return BuildConfiguration::Release;
#else
    return BuildConfiguration::Debug;
#endif
}

constexpr std::string_view DetectCompilerName() noexcept
{
#if defined(_MSC_VER)
    return std::string_view{"MSVC"};
#elif defined(__clang__)
    return std::string_view{"Clang"};
#elif defined(__GNUC__)
    return std::string_view{"GCC"};
#else
    return std::string_view{"Unknown"};
#endif
}

constexpr std::string_view DetectCompilerVersion() noexcept
{
#if defined(_MSC_VER)
#    if _MSC_VER >= 1940
    return std::string_view{"14.40+"};
#    elif _MSC_VER >= 1930
    return std::string_view{"14.30+"};
#    elif _MSC_VER >= 1920
    return std::string_view{"14.20+"};
#    elif _MSC_VER >= 1910
    return std::string_view{"14.10+"};
#    else
    return std::string_view{"Unknown"};
#    endif
#elif defined(__clang__)
    return std::string_view{__clang_version__};
#elif defined(__GNUC__)
#    if defined(__GNUC_PATCHLEVEL__)
#        define ELLINDYER_STRINGIFY_IMPL(x) #x
#        define ELLINDYER_STRINGIFY(x) ELLINDYER_STRINGIFY_IMPL(x)
    return std::string_view{
        ELLINDYER_STRINGIFY(__GNUC__) "." ELLINDYER_STRINGIFY(__GNUC_MINOR__) "." ELLINDYER_STRINGIFY(__GNUC_PATCHLEVEL__)};
#        undef ELLINDYER_STRINGIFY
#        undef ELLINDYER_STRINGIFY_IMPL
#    else
    return std::string_view{"Unknown"};
#    endif
#else
    return std::string_view{"Unknown"};
#endif
}

constexpr std::string_view DetectPlatform() noexcept
{
#if defined(_WIN32)
    return std::string_view{"Windows"};
#elif defined(__linux__)
    return std::string_view{"Linux"};
#elif defined(__APPLE__)
    return std::string_view{"macOS"};
#else
    return std::string_view{"Unknown"};
#endif
}

constexpr std::string_view DetectArchitecture() noexcept
{
#if defined(_M_X64) || defined(__x86_64__)
    return std::string_view{"x64"};
#elif defined(_M_IX86) || defined(__i386__)
    return std::string_view{"x86"};
#elif defined(_M_ARM64) || defined(__aarch64__)
    return std::string_view{"ARM64"};
#elif defined(_M_ARM) || defined(__arm__)
    return std::string_view{"ARM"};
#else
    return std::string_view{"Unknown"};
#endif
}

constexpr std::string_view DetectCppStandard() noexcept
{
#if defined(_MSVC_LANG)
#    if _MSVC_LANG >= 202302L
    return std::string_view{"C++23"};
#    elif _MSVC_LANG >= 202002L
    return std::string_view{"C++20"};
#    elif _MSVC_LANG >= 201703L
    return std::string_view{"C++17"};
#    elif _MSVC_LANG >= 201402L
    return std::string_view{"C++14"};
#    else
    return std::string_view{"C++11 or earlier"};
#    endif
#elif defined(__cplusplus)
#    if __cplusplus >= 202302L
    return std::string_view{"C++23"};
#    elif __cplusplus >= 202002L
    return std::string_view{"C++20"};
#    elif __cplusplus >= 201703L
    return std::string_view{"C++17"};
#    elif __cplusplus >= 201402L
    return std::string_view{"C++14"};
#    else
    return std::string_view{"C++11 or earlier"};
#    endif
#else
    return std::string_view{"Unknown"};
#endif
}

constexpr std::string_view DetectBuildDate() noexcept
{
    return std::string_view{__DATE__};
}

constexpr std::string_view DetectBuildTime() noexcept
{
    return std::string_view{__TIME__};
}

} // namespace

BuildInfo GetBuildInfo() noexcept
{
    BuildInfo info{};
    info.configuration     = DetectBuildConfiguration();
    info.compiler_name     = DetectCompilerName();
    info.compiler_version  = DetectCompilerVersion();
    info.platform          = DetectPlatform();
    info.architecture      = DetectArchitecture();
    info.cpp_standard      = DetectCppStandard();
    info.build_date        = DetectBuildDate();
    info.build_time        = DetectBuildTime();
    return info;
}

std::string_view GetBuildConfigurationName(BuildConfiguration configuration) noexcept
{
    switch (configuration)
    {
    case BuildConfiguration::Debug:         return std::string_view{"Debug"};
    case BuildConfiguration::Release:       return std::string_view{"Release"};
    case BuildConfiguration::RelWithDebInfo:return std::string_view{"RelWithDebInfo"};
    case BuildConfiguration::MinSizeRel:    return std::string_view{"MinSizeRel"};
    case BuildConfiguration::Unknown:       return std::string_view{"Unknown"};
    }
    return std::string_view{"Unknown"};
}

std::string GetBuildInfoSummary()
{
    const BuildInfo info = GetBuildInfo();
    std::ostringstream stream;
    stream << "Configuration: " << GetBuildConfigurationName(info.configuration) << '\n';
    stream << "Compiler: "      << info.compiler_name << ' ' << info.compiler_version << '\n';
    stream << "Platform: "      << info.platform << ' ' << info.architecture << '\n';
    stream << "Standard: "      << info.cpp_standard << '\n';
    stream << "Build date: "    << info.build_date << ' ' << info.build_time;
    return stream.str();
}

} // namespace ellindyer::core
