#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "App/ApplicationContext.hpp"
#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "UI/Menu/ApplicationMenuBuilder.hpp"
#include "UI/Shell/ShellLayout.hpp"
#include "UI/Shell/ShellWindow.hpp"
#include "UI/Splash/SplashScreen.hpp"
#include "UI/StatusBar/ApplicationStatusBarBuilder.hpp"
#include "UI/Toolbar/ApplicationToolbarBuilder.hpp"
#include "UI/UIHost.hpp"

namespace ellindyer::app
{

enum class ApplicationState : std::uint8_t
{
    Uninitialized,
    Initializing,
    Running,
    ShuttingDown,
    Terminated
};

class ApplicationLifecycle
{
public:
    ApplicationLifecycle();
    ~ApplicationLifecycle();

    ApplicationLifecycle(const ApplicationLifecycle&) = delete;
    ApplicationLifecycle& operator=(const ApplicationLifecycle&) = delete;
    ApplicationLifecycle(ApplicationLifecycle&&) noexcept = delete;
    ApplicationLifecycle& operator=(ApplicationLifecycle&&) noexcept = delete;

    [[nodiscard]] ellindyer::core::Result<void> Initialize();
    [[nodiscard]] ellindyer::core::Result<int> Run();
    void Shutdown();

    [[nodiscard]] ApplicationState GetState() const noexcept;
    [[nodiscard]] const ApplicationContext& GetContext() const noexcept;

private:
    [[nodiscard]] ellindyer::core::Result<void> InitializeLogging();
    [[nodiscard]] ellindyer::core::Result<void> InitializeUI();

    void ConfigureShell();
    void ConfigureMenuBar();
    void ConfigureToolbar();
    void ConfigureStatusBar();

    void HandleMenuCommand(const std::string& identifier);
    void HandleToolbarCommand(const std::string& identifier);
    void HandleStatusBarCommand(const std::string& identifier);

    ellindyer::ui::menu::ApplicationMenuHandlers BuildMenuHandlers();
    ellindyer::ui::menu::ApplicationMenuState BuildMenuState() const;
    ellindyer::ui::toolbar::ApplicationToolbarHandlers BuildToolbarHandlers();
    ellindyer::ui::toolbar::ApplicationToolbarState BuildToolbarState() const;
    ellindyer::ui::statusbar::ApplicationStatusBarState BuildStatusBarState() const;

    void UpdateStatusBar();

    void RunSplashStage();
    void RunMainLoop();
    void RenderMainFrame();
    void EmitStartupDiagnostics();
    void EmitShutdownDiagnostics();

    ApplicationContext context_;
    ApplicationState   state_ = ApplicationState::Uninitialized;
    bool               logging_initialized_ = false;
    bool               should_exit_         = false;

    std::unique_ptr<ellindyer::ui::UIHost> ui_host_;

    ellindyer::ui::splash::SplashScreen    splash_;
    ellindyer::ui::shell::ShellWindow      shell_window_;
    ellindyer::ui::shell::ShellLayout      shell_layout_;

    std::string                            status_message_ = "Ready";

    bool                                   splash_stage_complete_ = false;
};

} // namespace ellindyer::app
