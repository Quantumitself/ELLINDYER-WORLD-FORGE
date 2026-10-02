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
#include "UI/Toolbar/ApplicationToolbarBuilder.hpp"

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
    ConfigureToolbar();
    ConfigureStatusBar();

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
    shell_configuration.show_toolbar    = true;
    shell_configuration.show_status_bar = true;
    shell_configuration.show_dockspace  = false;

    shell_window_.Configure(shell_configuration);
    shell_layout_.Configure();

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

void ApplicationLifecycle::ConfigureToolbar()
{
    ellindyer::ui::toolbar::Toolbar& toolbar = shell_window_.GetToolbar();

    ellindyer::ui::toolbar::ToolbarStyle style{};
    style.height          = 34.0f;
    style.padding_x       = 10.0f;
    style.padding_y       = 4.0f;
    style.item_spacing    = 4.0f;
    style.show_separators = true;

    toolbar.SetStyle(style);
    toolbar.SetCommandHandler(
        [this](const std::string& identifier)
        {
            HandleToolbarCommand(identifier);
        });

    const ellindyer::ui::toolbar::ApplicationToolbarState state = BuildToolbarState();
    const ellindyer::ui::toolbar::ApplicationToolbarHandlers handlers = BuildToolbarHandlers();

    ellindyer::ui::toolbar::ApplicationToolbarBuilder::Build(toolbar, state, handlers);
}

void ApplicationLifecycle::ConfigureStatusBar()
{
    ellindyer::ui::statusbar::StatusBar& status_bar = shell_window_.GetStatusBar();

    status_bar.SetSegmentClickedHandler(
        [this](const std::string& identifier)
        {
            HandleStatusBarCommand(identifier);
        });

    const ellindyer::ui::statusbar::ApplicationStatusBarState state = BuildStatusBarState();
    ellindyer::ui::statusbar::ApplicationStatusBarBuilder::Build(status_bar, state);
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
        shell_window_.GetToolbar().SetItemChecked("toolbar.toggle_project_explorer", next);
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
        shell_window_.GetToolbar().SetItemChecked("toolbar.toggle_inspector", next);
        return;
    }

    if (identifier == "view.toggle_status_bar")
    {
        const bool next = !shell_window_.IsStatusBarVisible();
        shell_window_.SetStatusBarVisible(next);
        menu_bar.SetItemChecked("View", identifier, next);
        return;
    }

    if (identifier == "view.reset_layout")
    {
        shell_layout_.ResetLayout();

        menu_bar.SetItemChecked("View", "view.toggle_project_explorer", true);
        menu_bar.SetItemChecked("View", "view.toggle_workspace", true);
        menu_bar.SetItemChecked("View", "view.toggle_inspector", true);
        menu_bar.SetItemChecked("View", "view.toggle_status_bar", true);

        shell_window_.SetStatusBarVisible(true);

        ellindyer::ui::toolbar::Toolbar& toolbar = shell_window_.GetToolbar();
        toolbar.SetItemChecked("toolbar.toggle_project_explorer", true);
        toolbar.SetItemChecked("toolbar.toggle_inspector", true);
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

void ApplicationLifecycle::HandleToolbarCommand(const std::string& identifier)
{
    if (identifier == "toolbar.toggle_project_explorer")
    {
        HandleMenuCommand("view.toggle_project_explorer");
        return;
    }

    if (identifier == "toolbar.toggle_inspector")
    {
        HandleMenuCommand("view.toggle_inspector");
        return;
    }

    ELLINDYER_LOG_INFO("Toolbar command: " + identifier);
}

void ApplicationLifecycle::HandleStatusBarCommand(const std::string& identifier)
{
    if (identifier == "status.diagnostics")
    {
        HandleMenuCommand("tools.open_diagnostics");
        return;
    }

    if (identifier == "status.selection")
    {
        HandleMenuCommand("view.toggle_inspector");
        return;
    }

    ELLINDYER_LOG_INFO("Status bar command: " + identifier);
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
    state.show_status_bar       = shell_window_.IsStatusBarVisible();
    return state;
}

ellindyer::ui::toolbar::ApplicationToolbarHandlers ApplicationLifecycle::BuildToolbarHandlers()
{
    ellindyer::ui::toolbar::ApplicationToolbarHandlers handlers{};

    handlers.on_new_project = []()
    {
        ELLINDYER_LOG_INFO("Toolbar action: file.new_project");
    };

    handlers.on_open_project = []()
    {
        ELLINDYER_LOG_INFO("Toolbar action: file.open_project");
    };

    handlers.on_save_project = []()
    {
        ELLINDYER_LOG_INFO("Toolbar action: file.save_project");
    };

    handlers.on_undo = []()
    {
        ELLINDYER_LOG_INFO("Toolbar action: edit.undo");
    };

    handlers.on_redo = []()
    {
        ELLINDYER_LOG_INFO("Toolbar action: edit.redo");
    };

    handlers.on_toggle_project_explorer = [this]()
    {
        HandleToolbarCommand("toolbar.toggle_project_explorer");
    };

    handlers.on_toggle_inspector = [this]()
    {
        HandleToolbarCommand("toolbar.toggle_inspector");
    };

    handlers.on_validate_project = []()
    {
        ELLINDYER_LOG_INFO("Toolbar action: tools.validate_project");
    };

    handlers.on_open_search = []()
    {
        ELLINDYER_LOG_INFO("Toolbar action: tools.open_search");
    };

    handlers.on_export_universal = []()
    {
        ELLINDYER_LOG_INFO("Toolbar action: export.universal");
    };

    return handlers;
}

ellindyer::ui::toolbar::ApplicationToolbarState ApplicationLifecycle::BuildToolbarState() const
{
    ellindyer::ui::toolbar::ApplicationToolbarState state{};
    state.has_project           = false;
    state.can_undo              = false;
    state.can_redo              = false;
    state.show_project_explorer = shell_layout_.IsProjectExplorerVisible();
    state.show_inspector        = shell_layout_.IsInspectorVisible();
    return state;
}

ellindyer::ui::statusbar::ApplicationStatusBarState ApplicationLifecycle::BuildStatusBarState() const
{
    ellindyer::ui::statusbar::ApplicationStatusBarState state{};
    state.message               = status_message_;
    state.has_project           = false;
    state.project_name          = "";
    state.entity_count          = 0;
    state.diagnostic_errors     = 0;
    state.diagnostic_warnings   = 0;
    state.selection_summary     = "";
    state.frame_time_ms         = 0.0f;
    state.frames_per_second     = 0.0f;
    return state;
}

void ApplicationLifecycle::UpdateStatusBar()
{
    const ImGuiIO& io = ImGui::GetIO();

    ellindyer::ui::statusbar::ApplicationStatusBarState state = BuildStatusBarState();
    state.frame_time_ms     = io.DeltaTime * 1000.0f;
    state.frames_per_second = io.Framerate;

    ellindyer::ui::statusbar::ApplicationStatusBarBuilder::Update(
        shell_window_.GetStatusBar(), state);
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

    UpdateStatusBar();

    shell_window_.RenderTop(fonts);
    shell_layout_.Render(fonts,
                         shell_window_.GetTopConsumedHeight(),
                         shell_window_.GetBottomConsumedHeight());
    shell_window_.RenderBottom(fonts);
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
