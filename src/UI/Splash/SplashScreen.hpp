#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <imgui.h>

#include "UI/Branding/LogoTexture.hpp"
#include "UI/Fonts/FontManager.hpp"

namespace ellindyer::ui::splash
{

struct SplashScreenConfiguration
{
    float minimum_duration_seconds = 1.75f;
    float maximum_duration_seconds = 4.00f;
    bool  show_progress            = true;
    bool  show_status_text         = true;
};

class SplashScreen
{
public:
    SplashScreen();
    ~SplashScreen();

    SplashScreen(const SplashScreen&) = delete;
    SplashScreen& operator=(const SplashScreen&) = delete;
    SplashScreen(SplashScreen&&) noexcept = delete;
    SplashScreen& operator=(SplashScreen&&) noexcept = delete;

    void Configure(const SplashScreenConfiguration& configuration);

    void Reset(const std::string& application_name,
               const std::string& application_tagline,
               const std::string& application_version);

    void SetStatus(std::string_view status_message);

    void AddStep(std::string_view step_description);

    void MarkStepCompleted(std::string_view step_description);

    void SetProgress(float normalized_progress);

    void Complete();

    [[nodiscard]] bool IsComplete() const noexcept;

    [[nodiscard]] bool ShouldAdvance() const noexcept;

    [[nodiscard]] float ElapsedSeconds() const noexcept;

    void Render(ImFont* title_font,
                ImFont* heading_font,
                ImFont* regular_font,
                ImFont* small_font,
                const ellindyer::ui::branding::LogoTexture& logo);

private:
    SplashScreenConfiguration configuration_{};
    std::string               application_name_;
    std::string               application_tagline_;
    std::string               application_version_;
    std::string               status_message_;
    std::vector<std::string>  steps_;
    std::vector<bool>         step_states_;
    std::size_t               current_step_ = 0;
    float                     progress_     = 0.0f;
    bool                      completed_    = false;
    std::chrono::steady_clock::time_point start_time_{};
};

} // namespace ellindyer::ui::splash
