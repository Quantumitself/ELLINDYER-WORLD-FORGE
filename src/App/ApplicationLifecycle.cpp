#include "App/ApplicationLifecycle.hpp"

#include <cstdio>
#include <string>

#include <imgui.h>

#include "Core/Error.hpp"
#include "Core/ErrorCode.hpp"
#include "Core/LogBootstrap.hpp"
#include "Core/LogConfiguration.hpp"
#include "Core/LogLevel.hpp"
#include "Core/Logger.hpp"
#include "Core/LoggerExtensions.hpp"
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

constexpr const char*  kLogFileName      = "EllindyerWorldForge.log";
constexpr std::size_t  kLogMaxSizeBytes  = 4U * 1024U * 1024U;
constexpr std::size_t  kLogMaxFiles      = 4;
constexpr std::uint32_t kDefaultWindowWidth  = 1400U;
constexpr std::uint32_t kDefaultWindowHeight = 900U;

void RenderFrameContent(const ellindyer::core::ApplicationInfo& info)
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(io.DisplaySize);

    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24.0f, 24.0f));
    ImGui::Begin("##WorldForgeFrame", nullptr, flags);

    ImGui::Text("%s %s", info.name.c_str(), info.version.c_str());
    ImGui::Separator();
    ImGui::TextUnformatted("Main workspace placeholder.");
    ImGui::TextUnformatted("Splash stage completed.");
    ImGui::TextUnformatted("DirectX 11 rendering active.");
    ImGui::TextUnformatted("Dear ImGui context initialized.");
    ImGui::Text("Client area: %ux%u",
                static_cast<unsigned>(io.DisplaySize.x),
                static_cast<unsigned>(io.DisplaySize.y));
    ImGui::Text("Frame time: %.3f ms (%.1f FPS)",
                static_cast<double>(io.DeltaTime) * 1000.0,
                io.Framerate > 0.0f ? static_cast<double>(io.Framerate) : 0.0);

    ImGui::End();
    ImGui::PopStyleVar();
}

} // namespace

ApplicationLifecycle::ApplicationLifecycle()
    : ui_host_(std::make_unique<ellindyer::ui::UIHost>())
{
}

ApplicationLifecycle::~ApplicationLifecycle()
{
    if (state_ != ApplicationState::Terminated)
    {
        Shutdown();
    }
}

ellindyer::core::Result<void> ApplicationLifecycle::Initialize()
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (state_ != ApplicationState::Uninitialized)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "Application is already initialized."));
    }

    state_ = ApplicationState::Initializing;

    const Result<void> logging_result = InitializeLogging();
    if (logging_result.HasError())
    {
        state_ = ApplicationState::Uninitialized;
        return logging_result;
    }

    EmitStartupDiagnostics();

    const Result<void> ui_result = InitializeUI();
    if (ui_result.HasError())
    {
        Shutdown();
        state_ = ApplicationState::Uninitialized;
        return ui_result;
    }

    state_ = ApplicationState::Running;
    return Result<void>{};
}

ellindyer::core::Result<int> ApplicationLifecycle::Run()
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (state_ != ApplicationState::Running)
    {
        return Result<int>(MakeError(ErrorCode::InvalidState,
                                     "Application is not in the Running state."));
    }

    if (!ui_host_ || !ui_host_->IsInitialized())
    {
        return Result<int>(MakeError(ErrorCode::InvalidState,
                                     "UI host is not initialized."));
    }

    ELLINDYER_LOG_INFO("Application main loop entered");
    ui_host_->ShowWindow();

    RunSplashStage();
    RunMainLoop();

    ELLINDYER_LOG_INFO("Application main loop exited");

    return Result<int>(0);
}

void ApplicationLifecycle::Shutdown()
{
    if (state_ == ApplicationState::Terminated)
    {
        return;
    }

    state_ = ApplicationState::ShuttingDown;

    if (ui_host_)
    {
        ui_host_->Shutdown();
    }

    EmitShutdownDiagnostics();

    if (logging_initialized_)
    {
        ellindyer::core::LogBootstrap::Shutdown();
        logging_initialized_ = false;
    }

    state_ = ApplicationState::Terminated;

    std::fflush(stdout);
    std::fflush(stderr);
}

ApplicationState ApplicationLifecycle::GetState() const noexcept
{
    return state_;
}

const ApplicationContext& ApplicationLifecycle::GetContext() const noexcept
{
    return context_;
}

ellindyer::core::Result<void> ApplicationLifecycle::InitializeLogging()
{
    using ellindyer::core::LogBootstrap;
    using ellindyer::core::LogConfiguration;
    using ellindyer::core::LogConfigurationBuilder;
    using ellindyer::core::LogLevel;
    using ellindyer::core::MakeError;
    using ellindyer::core::ErrorCode;
    using ellindyer::core::Result;

    LogConfigurationBuilder builder;
    builder
        .WithLevel(LogLevel::Info)
        .WithConsole(true, true, false);

    if (context_.AreUserDirectoriesReady())
    {
        builder.WithFile(context_.GetLogsDirectory() / kLogFileName,
                         true, kLogMaxSizeBytes, kLogMaxFiles);
    }

    const LogConfiguration& configuration = builder.Build();
    if (!LogBootstrap::Initialize(configuration))
    {
        return Result<void>(MakeError(ErrorCode::SystemError,
                                      "Failed to initialize the logging subsystem."));
    }

    logging_initialized_ = true;
    return Result<void>{};
}

ellindyer::core::Result<void> ApplicationLifecycle::InitializeUI()
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (!ui_host_)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "UI host instance is not available."));
    }

    ellindyer::ui::UIHostDescription description{};
    description.window_title  = L"Ellindyer World Forge";
    description.window_width  = kDefaultWindowWidth;
    description.window_height = kDefaultWindowHeight;
    description.vsync         = true;
    description.centered      = true;
    description.resizable     = true;

    const Result<void> ui_result = ui_host_->Initialize(description);
    if (ui_result.HasError())
    {
        ELLINDYER_LOG_ERROR(ui_result.GetError().ToDiagnosticString());
        return ui_result;
    }

    ELLINDYER_LOG_INFO("UI host initialized");

    ellindyer::ui::splash::SplashScreenConfiguration splash_configuration{};
    splash_configuration.minimum_duration_seconds = 1.75f;
    splash_configuration.maximum_duration_seconds = 4.00f;
    splash_configuration.show_progress            = true;
    splash_configuration.show_status_text         = true;

    splash_.Configure(splash_configuration);
    splash_.Reset(context_.GetApplicationInfo().name,
                  context_.GetApplicationInfo().tagline,
                  context_.GetApplicationInfo().version);

    splash_.AddStep("Resolving paths");
    splash_.MarkStepCompleted("Resolving paths");
    splash_.AddStep("Initializing logging");
    splash_.MarkStepCompleted("Initializing logging");
    splash_.AddStep("Loading branding assets");
    splash_.MarkStepCompleted("Loading branding assets");
    splash_.AddStep("Loading fonts");
    splash_.MarkStepCompleted("Loading fonts");
    splash_.AddStep("Loading application logo");
    splash_.MarkStepCompleted("Loading application logo");
    splash_.AddStep("Initializing UI framework");
    splash_.MarkStepCompleted("Initializing UI framework");
    splash_.AddStep("Ready");

    splash_.SetProgress(0.95f);
    splash_.SetStatus("Ready.");

    return Result<void>{};
}

void ApplicationLifecycle::RunSplashStage()
{
    using ellindyer::ui::fonts::FontSet;
    using ellindyer::ui::branding::LogoTexture;

    const FontSet& fonts = ui_host_->GetFonts();
    const LogoTexture& logo = ui_host_->GetLogo();

    while (!splash_.IsComplete())
    {
        if (!ui_host_->PumpMessages())
        {
            splash_.Complete();
            return;
        }

        if (ui_host_->ShouldClose())
        {
            splash_.Complete();
            return;
        }

        ui_host_->BeginFrame();

        splash_.Render(fonts.title_medium,
                       fonts.heading_bold,
                       fonts.default_regular,
                       fonts.small_regular,
                       logo);

        ui_host_->EndFrame();

        const ellindyer::core::Result<void> present_result = ui_host_->Present();
        if (present_result.HasError())
        {
            ELLINDYER_LOG_ERROR(present_result.GetError().ToDiagnosticString());
            splash_.Complete();
            ui_host_->RequestClose();
            return;
        }

        if (splash_.ShouldAdvance())
        {
            splash_.Complete();
        }
    }

    splash_stage_complete_ = true;
    ELLINDYER_LOG_INFO("Splash stage completed");
}

void ApplicationLifecycle::RunMainLoop()
{
    while (!ui_host_->ShouldClose())
    {
        if (!ui_host_->PumpMessages())
        {
            break;
        }

        if (ui_host_->ShouldClose())
        {
            break;
        }

        ui_host_->BeginFrame();
        RenderMainFrame();
        ui_host_->EndFrame();

        const ellindyer::core::Result<void> present_result = ui_host_->Present();
        if (present_result.HasError())
        {
            ELLINDYER_LOG_ERROR(present_result.GetError().ToDiagnosticString());
            ui_host_->RequestClose();
        }
    }
}

void ApplicationLifecycle::RenderMainFrame()
{
    RenderFrameContent(context_.GetApplicationInfo());
}

void ApplicationLifecycle::EmitStartupDiagnostics()
{
    using ellindyer::core::GetLogger;
    using ellindyer::core::LoggerExtensions;
    using ellindyer::core::LogLevel;

    const ellindyer::core::ApplicationInfo& info  = context_.GetApplicationInfo();
    const ellindyer::core::BuildInfo&       build = context_.GetBuildInfo();

    LoggerExtensions::LogSection(GetLogger(), std::string_view{"Application Startup"});

    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "application",    info.name);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "version",        info.version);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "tagline",        info.tagline);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "organization",   info.organization);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "configuration",
        ellindyer::core::GetBuildConfigurationName(build.configuration));
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "compiler",       build.compiler_name);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "compiler_ver",   build.compiler_version);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "platform",       build.platform);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "architecture",   build.architecture);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "cpp_standard",   build.cpp_standard);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "build_date",     build.build_date);
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "build_time",     build.build_time);

    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "exe_dir",
        context_.GetExecutableDirectory().generic_string());
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "resources_dir",
        context_.GetResourcesDirectory().generic_string());
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "user_data_dir",
        context_.GetUserDataDirectory().generic_string());
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "logs_dir",
        context_.GetLogsDirectory().generic_string());
    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "config_dir",
        context_.GetConfigurationDirectory().generic_string());

    LoggerExtensions::LogKeyValue(GetLogger(), LogLevel::Info, "user_dirs_ready",
        context_.AreUserDirectoriesReady() ? std::string_view{"true"} : std::string_view{"false"});

    ELLINDYER_LOG_INFO("Application initialized");
}

void ApplicationLifecycle::EmitShutdownDiagnostics()
{
    ELLINDYER_LOG_INFO("Application shutdown initiated");
}

} // namespace ellindyer::app
