#pragma once

#include <cstdint>
#include <memory>

#include "App/ApplicationContext.hpp"
#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "UI/Splash/SplashScreen.hpp"
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

    void RunSplashStage();

    void RunMainLoop();

    void RenderMainFrame();

    void EmitStartupDiagnostics();

    void EmitShutdownDiagnostics();

    ApplicationContext context_;
    ApplicationState   state_ = ApplicationState::Uninitialized;
    bool               logging_initialized_ = false;

    std::unique_ptr<ellindyer::ui::UIHost> ui_host_;

    ellindyer::ui::splash::SplashScreen    splash_;
    bool                                   splash_stage_complete_ = false;
};

} // namespace ellindyer::app
