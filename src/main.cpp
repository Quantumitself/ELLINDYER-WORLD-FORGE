#include <cstdio>
#include <cstdlib>
#include <string>

#include "Core/ApplicationInfo.hpp"
#include "Core/ApplicationPaths.hpp"
#include "Core/BuildInfo.hpp"
#include "Core/Error.hpp"
#include "Core/ErrorCode.hpp"
#include "Core/ErrorLogging.hpp"
#include "Core/FileHelpers.hpp"
#include "Core/FileSystemSummary.hpp"
#include "Core/Identifier.hpp"
#include "Core/IdentifierFormatter.hpp"
#include "Core/IdentifierGenerator.hpp"
#include "Core/LogBootstrap.hpp"
#include "Core/LogConfiguration.hpp"
#include "Core/LogLevel.hpp"
#include "Core/Logger.hpp"
#include "Core/LoggerExtensions.hpp"
#include "Core/LogMacros.hpp"
#include "Core/Math/Geometry.hpp"
#include "Core/Math/MathConstants.hpp"
#include "Core/Math/MathUtils.hpp"
#include "Core/Math/Rect.hpp"
#include "Core/Math/Size.hpp"
#include "Core/Math/Transform2D.hpp"
#include "Core/Math/Vec2.hpp"
#include "Core/Math/Vec3.hpp"
#include "Core/PathHelpers.hpp"
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

bool InitializeLogging()
{
    using ellindyer::core::ApplicationPaths;
    using ellindyer::core::LogBootstrap;
    using ellindyer::core::LogConfiguration;
    using ellindyer::core::LogConfigurationBuilder;
    using ellindyer::core::LogLevel;

    const bool user_dirs_ready = ApplicationPaths::EnsureUserDirectoriesExist();
    if (!user_dirs_ready)
    {
        return false;
    }

    LogConfigurationBuilder builder;
    builder
        .WithLevel(LogLevel::Info)
        .WithConsole(true, true, false)
        .WithFile(ApplicationPaths::GetLogsDirectory() / "EllindyerWorldForge.log",
                  true, 4U * 1024U * 1024U, 4);

    const LogConfiguration& configuration = builder.Build();
    return LogBootstrap::Initialize(configuration);
}

void EmitStartupLogs()
{
    using ellindyer::core::ApplicationInfo;
    using ellindyer::core::BuildInfo;
    using ellindyer::core::GetApplicationInfo;
    using ellindyer::core::GetBuildInfo;
    using ellindyer::core::GetLogger;
    using ellindyer::core::LoggerExtensions;
    using ellindyer::core::LogLevel;

    const ApplicationInfo info = GetApplicationInfo();
    const BuildInfo build = GetBuildInfo();

    LoggerExtensions::LogSection(GetLogger(), std::string_view{"Startup"});
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "application", info.name);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "version", info.version);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "tagline", info.tagline);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "organization", info.organization);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "configuration",
        std::string_view{reinterpret_cast<const char*>(
            ellindyer::core::GetBuildConfigurationName(build.configuration).data()),
            ellindyer::core::GetBuildConfigurationName(build.configuration).size()});
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "compiler", build.compiler_name);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "platform", build.platform);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "architecture", build.architecture);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "cpp_standard", build.cpp_standard);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "build_date", build.build_date);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "build_time", build.build_time);

    ELLINDYER_LOG_INFO("Application startup complete");
}

void PrintFileHelpersInformation()
{
    using ellindyer::core::ApplicationPaths;
    using ellindyer::core::DirectorySummary;
    using ellindyer::core::FileHelpers;
    using ellindyer::core::FileSystemSummary;
    using ellindyer::core::PathHelpers;
    using ellindyer::core::Result;

    const std::filesystem::path user_dir = ApplicationPaths::GetUserDataDirectory();
    const std::filesystem::path scratch_dir = user_dir / "scratch";
    const std::filesystem::path test_file = scratch_dir / "hello.txt";

    std::printf("File helpers examples:%s", kLineEnding);

    const Result<void> dir_ready = FileHelpers::CreateDirectoryIfMissing(scratch_dir);
    std::printf("  create dir   : %s%s",
                dir_ready.HasValue() ? "ok" : dir_ready.GetError().ToDiagnosticString().c_str(),
                kLineEnding);

    const Result<void> write_ok = FileHelpers::WriteAllText(test_file, "Hello, World!");
    std::printf("  write text   : %s%s",
                write_ok.HasValue() ? "ok" : write_ok.GetError().ToDiagnosticString().c_str(),
                kLineEnding);

    const Result<std::string> read_back = FileHelpers::ReadAllText(test_file);
    std::printf("  read text    : %s (%s)%s",
                read_back.HasValue() ? "ok" : "error",
                read_back.HasValue() ? read_back.Value().c_str()
                                     : read_back.GetError().ToDiagnosticString().c_str(),
                kLineEnding);

    const Result<std::uintmax_t> file_size = FileHelpers::GetFileSize(test_file);
    std::printf("  file size    : %s%s",
                file_size.HasValue()
                    ? std::to_string(file_size.Value()).c_str()
                    : file_size.GetError().ToDiagnosticString().c_str(),
                kLineEnding);

    const bool exists = FileHelpers::Exists(test_file);
    std::printf("  exists       : %s%s",
                exists ? "true" : "false",
                kLineEnding);

    const std::string normalized = FileHelpers::NormalizeSeparators(test_file.string());
    std::printf("  normalized   : %s%s", normalized.c_str(), kLineEnding);

    const std::string base_name = PathHelpers::GetFileNameWithoutExtension(test_file);
    std::printf("  base name    : %s%s", base_name.c_str(), kLineEnding);

    const bool has_txt = PathHelpers::HasExtension(test_file, "txt");
    std::printf("  has .txt     : %s%s", has_txt ? "true" : "false", kLineEnding);

    const std::filesystem::path sanitized = PathHelpers::SanitizeFileName("my:file/name?.txt");
    std::printf("  sanitized    : %s%s", sanitized.generic_string().c_str(), kLineEnding);

    const bool within = PathHelpers::IsWithinDirectory(test_file, user_dir);
    std::printf("  within user  : %s%s", within ? "true" : "false", kLineEnding);

    const DirectorySummary summary = FileSystemSummary::Summarize(scratch_dir, false);
    std::printf("  summary      : files=%llu dirs=%llu size=%s%s",
                static_cast<unsigned long long>(summary.file_count),
                static_cast<unsigned long long>(summary.directory_count),
                FileSystemSummary::FormatBytes(summary.total_bytes).c_str(),
                kLineEnding);

    const Result<void> removed = FileHelpers::RemoveFile(test_file);
    std::printf("  remove file  : %s%s",
                removed.HasValue() ? "ok" : removed.GetError().ToDiagnosticString().c_str(),
                kLineEnding);
}

void PrintMathInformation()
{
    using namespace ellindyer::core::math;

    const Vec2f a(3.0f, 4.0f);
    const Vec2f b(1.0f, 2.0f);

    std::printf("Vec2 examples:%s", kLineEnding);
    std::printf("  a             : %s%s", a.ToString().c_str(), kLineEnding);
    std::printf("  b             : %s%s", b.ToString().c_str(), kLineEnding);
    std::printf("  a + b         : %s%s", (a + b).ToString().c_str(), kLineEnding);
    std::printf("  a - b         : %s%s", (a - b).ToString().c_str(), kLineEnding);
    std::printf("  a * 2         : %s%s", (a * 2.0f).ToString().c_str(), kLineEnding);
    std::printf("  a / 2         : %s%s", (a / 2.0f).ToString().c_str(), kLineEnding);
    std::printf("  length(a)     : %.4f%s", static_cast<double>(a.Length()), kLineEnding);
    std::printf("  distance(a,b) : %.4f%s", static_cast<double>(a.Distance(b)), kLineEnding);
    std::printf("  dot(a,b)      : %.4f%s", static_cast<double>(a.Dot(b)), kLineEnding);
    std::printf("  cross(a,b)    : %.4f%s", static_cast<double>(a.Cross(b)), kLineEnding);
    std::printf("  normalized(a) : %s%s", a.Normalized().ToString().c_str(), kLineEnding);
    std::printf("  perpendicular : %s%s", a.Perpendicular().ToString().c_str(), kLineEnding);
    std::printf("  angle(a)      : %.4f rad%s",
                static_cast<double>(a.Angle()), kLineEnding);

    const Vec3f p(1.0f, 2.0f, 3.0f);
    const Vec3f q(4.0f, 5.0f, 6.0f);

    std::printf("Vec3 examples:%s", kLineEnding);
    std::printf("  p             : %s%s", p.ToString().c_str(), kLineEnding);
    std::printf("  q             : %s%s", q.ToString().c_str(), kLineEnding);
    std::printf("  p + q         : %s%s", (p + q).ToString().c_str(), kLineEnding);
    std::printf("  p - q         : %s%s", (p - q).ToString().c_str(), kLineEnding);
    std::printf("  p * 3         : %s%s", (p * 3.0f).ToString().c_str(), kLineEnding);
    std::printf("  dot(p,q)      : %.4f%s", static_cast<double>(p.Dot(q)), kLineEnding);
    std::printf("  cross(p,q)    : %s%s", p.Cross(q).ToString().c_str(), kLineEnding);
    std::printf("  length(p)     : %.4f%s", static_cast<double>(p.Length()), kLineEnding);
    std::printf("  normalized(p) : %s%s", p.Normalized().ToString().c_str(), kLineEnding);

    const Rectf rect_a(0.0f, 0.0f, 100.0f, 100.0f);
    const Rectf rect_b(50.0f, 50.0f, 100.0f, 100.0f);

    std::printf("Rect examples:%s", kLineEnding);
    std::printf("  rect_a         : %s%s", rect_a.ToString().c_str(), kLineEnding);
    std::printf("  rect_b         : %s%s", rect_b.ToString().c_str(), kLineEnding);
    std::printf("  area(a)        : %.4f%s", static_cast<double>(rect_a.Area()), kLineEnding);
    std::printf("  center(a)      : %s%s", rect_a.Center().ToString().c_str(), kLineEnding);
    std::printf("  intersects     : %s%s",
                rect_a.Intersects(rect_b) ? "true" : "false", kLineEnding);
    std::printf("  intersection   : %s%s",
                rect_a.Intersection(rect_b).ToString().c_str(), kLineEnding);
    std::printf("  union          : %s%s",
                rect_a.Union(rect_b).ToString().c_str(), kLineEnding);
    std::printf("  contains(25,25): %s%s",
                rect_a.Contains(25.0f, 25.0f) ? "true" : "false", kLineEnding);

    const Sizeu image_size(2400U, 1200U);
    std::printf("Size examples:%s", kLineEnding);
    std::printf("  size           : %s%s", image_size.ToString().c_str(), kLineEnding);
    std::printf("  area           : %u%s", image_size.Area(), kLineEnding);
    std::printf("  aspect ratio   : %.4f%s",
                static_cast<double>(image_size.AspectRatio()), kLineEnding);

    Transform2Df transform{};
    transform.position = Vec2f(100.0f, 50.0f);
    transform.scale    = Vec2f(2.0f, 2.0f);
    transform.rotation = kHalfPiF;

    const Vec2f point(1.0f, 0.0f);
    const Vec2f transformed = transform.Apply(point);
    const Vec2f inverse_transformed = transform.InverseApply(transformed);

    std::printf("Transform2D examples:%s", kLineEnding);
    std::printf("  apply          : %s%s", transformed.ToString().c_str(), kLineEnding);
    std::printf("  inverse apply  : %s%s", inverse_transformed.ToString().c_str(), kLineEnding);
    std::printf("  round-trip ok  : %s%s",
                inverse_transformed.IsNearlyEqual(point, 0.001f) ? "true" : "false",
                kLineEnding);

    std::printf("Geometry examples:%s", kLineEnding);
    std::printf("  dist p->seg    : %.4f%s",
                static_cast<double>(DistancePointToSegment(
                    Vec2f(5.0f, 5.0f), Vec2f(0.0f, 0.0f), Vec2f(10.0f, 0.0f))),
                kLineEnding);
    std::printf("  seg intersect  : %s%s",
                SegmentsIntersect(Vec2f(0.0f, 0.0f), Vec2f(10.0f, 10.0f),
                                  Vec2f(0.0f, 10.0f), Vec2f(10.0f, 0.0f))
                    ? "true" : "false",
                kLineEnding);
    std::printf("  point in circle: %s%s",
                PointInCircle(Vec2f(3.0f, 4.0f), Vec2f(0.0f, 0.0f), 5.0f)
                    ? "true" : "false",
                kLineEnding);

    std::printf("Math utils examples:%s", kLineEnding);
    std::printf("  clamp(150,0,100)  : %.4f%s",
                static_cast<double>(Clamp<float>(150.0f, 0.0f, 100.0f)), kLineEnding);
    std::printf("  lerp(0,100,0.25)  : %.4f%s",
                static_cast<double>(Lerp<float>(0.0f, 100.0f, 0.25f)), kLineEnding);
    std::printf("  smoothstep(0,1,.5): %.4f%s",
                static_cast<double>(SmoothStep<float>(0.0f, 1.0f, 0.5f)), kLineEnding);
    std::printf("  deg->rad(180)     : %.4f%s",
                static_cast<double>(DegToRad<float>(180.0f)), kLineEnding);
    std::printf("  rad->deg(pi)      : %.4f%s",
                static_cast<double>(RadToDeg<float>(kPiF)), kLineEnding);
}

int RunApplication()
{
    const bool logging_ready = InitializeLogging();
    if (!logging_ready)
    {
        std::fprintf(stderr, "Failed to initialize logging.%s", kLineEnding);
        return EXIT_FAILURE;
    }

    PrintApplicationBanner();
    PrintBuildInformation();
    PrintPathInformation();
    PrintThirdPartyStatus();
    PrintIdentifierInformation();
    PrintErrorInformation();
    PrintFileHelpersInformation();
    PrintMathInformation();

    EmitStartupLogs();

    ELLINDYER_LOG_INFO("Application shutdown initiated");
    ellindyer::core::LogBootstrap::Shutdown();

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
