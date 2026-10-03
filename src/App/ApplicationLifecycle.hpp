#pragma once

#include <cstdint>
#include <memory>

#include "App/ApplicationContext.hpp"
#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Core/Settings/ApplicationSettings.hpp"
#include "UI/Dialogs/DialogHost.hpp"
#include "UI/Dialogs/NewProjectDialog.hpp"
#include "UI/Dialogs/OpenProjectDialog.hpp"
#include "UI/Dialogs/SaveProjectAsDialog.hpp"
#include "UI/Dialogs/UnsavedChangesDialog.hpp"
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

enum class PendingAction : std::uint8_t
{
    None,
    CloseProject,
    NewProject,
    OpenProject,
    ExitApplication
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

    [[nodiscard]] ellindyer::core::Result<void> LoadApplicationSettings();

    [[nodiscard]] ellindyer::core::Result<void> SaveApplicationSettings();

    [[nodiscard]] ellindyer::core::Result<void> InitializeUI();

    void ApplySettingsToUI();

    void CaptureUISettings();

    void ConfigureShell();

    void ConfigureMenuBar();

    void ConfigureToolbar();

    void ConfigureStatusBar();

    void ConfigureDialogs();

    void HandleMenuCommand(const std::string& identifier);

    void HandleToolbarCommand(const std::string& identifier);

    void HandleStatusBarCommand(const std::string& identifier);

    void HandleNewProjectRequest(
        const ellindyer::ui::dialogs::NewProjectRequest& request);

    void HandleOpenProjectRequest(
        const ellindyer::ui::dialogs::OpenProjectRequest& request);

    void HandleSaveProjectAsRequest(
        const ellindyer::ui::dialogs::SaveProjectAsRequest& request);

    void HandleUnsavedChangesChoice(
        ellindyer::ui::dialogs::UnsavedChangesChoice choice);

    void OpenNewProjectDialog();

    void OpenOpenProjectDialog();

    void OpenSaveProjectAsDialog();

    void PerformSaveProject();

    void PerformCloseProject();

    void PerformExitApplication();

    void PerformNewProject();

    void PerformOpenProject();

    void RequestGuardedAction(PendingAction action);

    [[nodiscard]] std::string DescribePendingAction(PendingAction action) const;

    [[nodiscard]] bool IsActionAllowedNow() const noexcept;

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
    bool               settings_loaded_     = false;
    bool               should_exit_         = false;

    PendingAction      pending_action_      = PendingAction::None;

    std::unique_ptr<ellindyer::ui::UIHost> ui_host_;

    ellindyer::ui::splash::SplashScreen    splash_;
    ellindyer::ui::shell::ShellWindow      shell_window_;
    ellindyer::ui::shell::ShellLayout      shell_layout_;

    ellindyer::ui::dialogs::DialogHost     dialog_host_;
    std::shared_ptr<ellindyer::ui::dialogs::NewProjectDialog>      new_project_dialog_;
    std::shared_ptr<ellindyer::ui::dialogs::OpenProjectDialog>     open_project_dialog_;
    std::shared_ptr<ellindyer::ui::dialogs::SaveProjectAsDialog>   save_project_as_dialog_;
    std::shared_ptr<ellindyer::ui::dialogs::UnsavedChangesDialog>  unsaved_changes_dialog_;

    std::string                            status_message_ = "Ready";

    bool                                   splash_stage_complete_ = false;
};

} // namespace ellindyer::app
