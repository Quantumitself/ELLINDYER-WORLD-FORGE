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
#include "UI/Menu/ApplicationMenuBuilder.hpp"

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

    ConfigureShell();
    ConfigureMenuBar();

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

void ApplicationLifecycle::ConfigureShell()
{
    ellindyer::ui::shell::ShellWindowConfiguration shell_configuration{};
    shell_configuration.title           = context_.GetApplicationInfo().name;
    shell_configuration.subtitle        = context_.GetApplicationInfo().tagline;
    shell_configuration.show_menu_bar   = true;
    shell_configuration.show_status_bar = true;
    shell_configuration.show_dockspace  = false;

    shell_window_.Configure(shell_configuration);

    ellindyer::ui::shell::ShellLayoutConfiguration layout_configuration{};
    layout_configuration.panels.left_width_fraction  = 0.20f;
    layout_configuration.panels.right_width_fraction = 0.22f;
    layout_configuration.panels.left_min_width       = 220.0f;
    layout_configuration.panels.right_min_width      = 260.0f;
    layout_configuration.panels.center_min_width     = 320.0f;
    layout_configuration.panels.panel_spacing        = 8.0f;

    shell_layout_.Configure(layout_configuration);

    shell_layout_.GetProjectExplorer().SetProjectName("Ellindyer World Forge");
    shell_layout_.GetProjectExplorer().AddRootEntry("World");
    shell_layout_.GetProjectExplorer().AddRootEntry("Schemas");
    shell_layout_.GetProjectExplorer().AddRootEntry("Entities");
    shell_layout_.GetProjectExplorer().AddRootEntry("Relationships");
    shell_layout_.GetProjectExplorer().AddRootEntry("Narrative");
    shell_layout_.GetProjectExplorer().AddRootEntry("Quests");
    shell_layout_.GetProjectExplorer().AddRootEntry("Rules");
    shell_layout_.GetProjectExplorer().AddRootEntry("Formulas");
    shell_layout_.GetProjectExplorer().AddRootEntry("Maps");
    shell_layout_.GetProjectExplorer().AddRootEntry("Assets");

    shell_layout_.GetWorkspace().SetTitle("Workspace");
    shell_layout_.GetWorkspace().SetDescription("Ellindyer World Forge");
    shell_layout_.GetWorkspace().AddHint(
        "The workspace hosts schema, entity, relationship, narrative, spatial,");
    shell_layout_.GetWorkspace().AddHint(
        "formula, rule, and validation surfaces as they become available.");

    shell_layout_.GetInspector().SetSelectionTitle("");
    shell_layout_.GetInspector().SetSelectionSubtitle("");
    shell_layout_.GetInspector().Clear();

    shell_layout_.GetStatusBar().SetLeftText("Ready");
    shell_layout_.GetStatusBar().SetMiddleText("");
    shell_layout_.GetStatusBar().SetRightText("");
}

void ApplicationLifecycle::ConfigureMenuBar()
{
    ellindyer::ui::menu::MenuBar& menu_bar = shell_window_.GetMenuBar();

    menu_bar.SetCommandHandler(
        [this](const std::string& identifier)
        {
            HandleMenuCommand(identifier);
        });

    const ellindyer::ui::menu::ApplicationMenuState state = BuildMenuState();
    const ellindyer::ui::menu::ApplicationMenuHandlers handlers = BuildMenuHandlers();

    ellindyer::ui::menu::ApplicationMenuBuilder::Build(menu_bar, state, handlers);
}

void ApplicationLifecycle::HandleMenuCommand(const std::string& identifier)
{
    ELLINDYER_LOG_INFO("Menu command: " + identifier);

    ellindyer::ui::menu::MenuBar& menu_bar = shell_window_.GetMenuBar();

    if (identifier == "view.toggle_project_explorer")
    {
        const bool next = !shell_layout_.IsProjectExplorerVisible();
        shell_layout_.SetProjectExplorerVisible(next);
        menu_bar.SetItemChecked("View", identifier, next);
        return;
    }

    if (identifier == "view.toggle_workspace")
    {
        const bool next = !shell_layout_.IsWorkspaceVisible();
        shell_layout_.SetWorkspaceVisible(next);
        menu_bar.SetItemChecked("View", identifier, next);
        return;
    }

    if (identifier == "view.toggle_inspector")
    {
        const bool next = !shell_layout_.IsInspectorVisible();
        shell_layout_.SetInspectorVisible(next);
        menu_bar.SetItemChecked("View", identifier, next);
        return;
    }

    if (identifier == "view.toggle_status_bar")
    {
        const bool next = !shell_layout_.IsStatusBarVisible();
        shell_layout_.SetStatusBarVisible(next);
        menu_bar.SetItemChecked("View", identifier, next);
        return;
    }

    if (identifier == "view.reset_layout")
    {
        shell_layout_.ResetVisibility();

        menu_bar.SetItemChecked("View", "view.toggle_project_explorer", true);
        menu_bar.SetItemChecked("View", "view.toggle_workspace", true);
        menu_bar.SetItemChecked("View", "view.toggle_inspector", true);
        menu_bar.SetItemChecked("View", "view.toggle_status_bar", true);

        ellindyer::ui::shell::ShellLayoutConfiguration layout_configuration{};
        layout_configuration.panels.left_width_fraction  = 0.20f;
        layout_configuration.panels.right_width_fraction = 0.22f;
        layout_configuration.panels.left_min_width       = 220.0f;
        layout_configuration.panels.right_min_width      = 260.0f;
        layout_configuration.panels.center_min_width     = 320.0f;
        layout_configuration.panels.panel_spacing        = 8.0f;
        shell_layout_.Configure(layout_configuration);
        return;
    }

    if (identifier == "file.exit")
    {
        should_exit_ = true;
        if (ui_host_)
        {
            ui_host_->RequestClose();
        }
        return;
    }
}

ellindyer::ui::menu::ApplicationMenuHandlers ApplicationLifecycle::BuildMenuHandlers()
{
    ellindyer::ui::menu::ApplicationMenuHandlers handlers{};

    handlers.on_exit_application = [this]()
    {
        should_exit_ = true;
        if (ui_host_)
        {
            ui_host_->RequestClose();
        }
    };

    handlers.on_toggle_project_explorer = [this]()
    {
        HandleMenuCommand("view.toggle_project_explorer");
    };

    handlers.on_toggle_workspace = [this]()
    {
        HandleMenuCommand("view.toggle_workspace");
    };

    handlers.on_toggle_inspector = [this]()
    {
        HandleMenuCommand("view.toggle_inspector");
    };

    handlers.on_toggle_status_bar = [this]()
    {
        HandleMenuCommand("view.toggle_status_bar");
    };

    handlers.on_reset_layout = [this]()
    {
        HandleMenuCommand("view.reset_layout");
    };

    const auto log_only = [](const char* identifier)
    {
        return [identifier]()
        {
            ELLINDYER_LOG_INFO(std::string("Menu action: ") + identifier);
        };
    };

    handlers.on_new_project       = log_only("file.new_project");
    handlers.on_open_project      = log_only("file.open_project");
    handlers.on_save_project      = log_only("file.save_project");
    handlers.on_save_project_as   = log_only("file.save_project_as");
    handlers.on_close_project     = log_only("file.close_project");

    handlers.on_undo              = log_only("edit.undo");
    handlers.on_redo              = log_only("edit.redo");
    handlers.on_cut               = log_only("edit.cut");
    handlers.on_copy              = log_only("edit.copy");
    handlers.on_paste             = log_only("edit.paste");
    handlers.on_delete_selection  = log_only("edit.delete");

    handlers.on_new_schema        = log_only("project.new_schema");
    handlers.on_new_entity        = log_only("project.new_entity");
    handlers.on_new_relationship  = log_only("project.new_relationship");
    handlers.on_new_map           = log_only("project.new_map");
    handlers.on_import_asset      = log_only("project.import_asset");

    handlers.on_validate_project  = log_only("tools.validate_project");
    handlers.on_open_diagnostics  = log_only("tools.open_diagnostics");
    handlers.on_open_search       = log_only("tools.open_search");

    handlers.on_export_universal  = log_only("export.universal");
    handlers.on_export_unreal     = log_only("export.unreal");
    handlers.on_export_unity      = log_only("export.unity");
    handlers.on_export_godot      = log_only("export.godot");

    handlers.on_show_about        = log_only("help.about");
    handlers.on_show_documentation = log_only("help.documentation");

    return handlers;
}

ellindyer::ui::menu::ApplicationMenuState ApplicationLifecycle::BuildMenuState() const
{
    ellindyer::ui::menu::ApplicationMenuState state{};
    state.has_project           = false;
    state.has_selection         = false;
    state.has_clipboard         = false;
    state.show_project_explorer = shell_layout_.IsProjectExplorerVisible();
    state.show_workspace        = shell_layout_.IsWorkspaceVisible();
    state.show_inspector        = shell_layout_.IsInspectorVisible();
    state.show_status_bar       = shell_layout_.IsStatusBarVisible();
    return state;
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
    while (!ui_host_->ShouldClose() && !should_exit_)
    {
        if (!ui_host_->PumpMessages())
        {
            break;
        }

        if (ui_host_->ShouldClose() || should_exit_)
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
    const ellindyer::ui::fonts::FontSet& fonts = ui_host_->GetFonts();

    const ImGuiIO& io = ImGui::GetIO();

    shell_layout_.GetStatusBar().SetMiddleText(
        "Frame: " + std::to_string(static_cast<int>(io.DeltaTime * 1000.0f)) + " ms");
    shell_layout_.GetStatusBar().SetRightText(
        "FPS: " + std::to_string(static_cast<int>(io.Framerate)));

    shell_window_.Render(fonts);
    shell_layout_.Render(fonts);
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
