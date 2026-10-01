#include "UI/Splash/SplashScreen.hpp"

#include <algorithm>
#include <string>

#include <imgui.h>

namespace ellindyer::ui::splash
{

namespace
{

constexpr float kLogoWidthMax  = 192.0f;
constexpr float kLogoHeightMax = 192.0f;
constexpr float kWindowWidth   = 560.0f;
constexpr float kWindowHeight  = 360.0f;
constexpr const char* kSplashWindowName = "##WorldForgeSplash";

ImVec4 ColorTextPrimary()   { return ImVec4(1.00f, 1.00f, 1.00f, 1.00f); }
ImVec4 ColorTextSecondary() { return ImVec4(0.68f, 0.68f, 0.68f, 1.00f); }
ImVec4 ColorTextMuted()     { return ImVec4(0.45f, 0.45f, 0.45f, 1.00f); }
ImVec4 ColorAccent()        { return ImVec4(0.92f, 0.92f, 0.92f, 1.00f); }
ImVec4 ColorDim()           { return ImVec4(0.24f, 0.24f, 0.24f, 1.00f); }

void RenderCenteredText(ImFont* font,
                        const std::string& text,
                        float window_width,
                        const ImVec4& color)
{
    if (font != nullptr)
    {
        ImGui::PushFont(font, font->LegacySize);
    }

    const float text_width = ImGui::CalcTextSize(text.c_str()).x;
    const float cursor_x   = (window_width - text_width) * 0.5f;
    if (cursor_x > 0.0f)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursor_x);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TextUnformatted(text.c_str());
    ImGui::PopStyleColor();

    if (font != nullptr)
    {
        ImGui::PopFont();
    }
}

} // namespace

SplashScreen::SplashScreen() = default;

SplashScreen::~SplashScreen() = default;

void SplashScreen::Configure(const SplashScreenConfiguration& configuration)
{
    configuration_ = configuration;
    if (configuration_.minimum_duration_seconds < 0.0f)
    {
        configuration_.minimum_duration_seconds = 0.0f;
    }
    if (configuration_.maximum_duration_seconds < configuration_.minimum_duration_seconds)
    {
        configuration_.maximum_duration_seconds = configuration_.minimum_duration_seconds;
    }
}

void SplashScreen::Reset(const std::string& application_name,
                         const std::string& application_tagline,
                         const std::string& application_version)
{
    application_name_    = application_name;
    application_tagline_ = application_tagline;
    application_version_ = application_version;
    status_message_      = "Initializing...";
    steps_.clear();
    step_states_.clear();
    current_step_ = 0;
    progress_     = 0.0f;
    completed_    = false;
    start_time_   = std::chrono::steady_clock::now();
}

void SplashScreen::SetStatus(std::string_view status_message)
{
    status_message_ = std::string(status_message);
}

void SplashScreen::AddStep(std::string_view step_description)
{
    steps_.emplace_back(step_description);
    step_states_.push_back(false);
    current_step_ = steps_.size() - 1;
    status_message_ = std::string(step_description);
}

void SplashScreen::MarkStepCompleted(std::string_view step_description)
{
    const std::string target(step_description);
    for (std::size_t index = 0; index < steps_.size(); ++index)
    {
        if (steps_[index] == target)
        {
            step_states_[index] = true;
            if (index + 1 < steps_.size())
            {
                current_step_ = index + 1;
                status_message_ = steps_[current_step_];
            }
            return;
        }
    }
}

void SplashScreen::SetProgress(float normalized_progress)
{
    progress_ = std::clamp(normalized_progress, 0.0f, 1.0f);
}

void SplashScreen::Complete()
{
    completed_ = true;
    progress_  = 1.0f;
}

bool SplashScreen::IsComplete() const noexcept
{
    return completed_;
}

bool SplashScreen::ShouldAdvance() const noexcept
{
    if (completed_)
    {
        return true;
    }

    const float elapsed = ElapsedSeconds();
    if (elapsed >= configuration_.maximum_duration_seconds)
    {
        return true;
    }

    if (elapsed < configuration_.minimum_duration_seconds)
    {
        return false;
    }

    if (configuration_.show_progress)
    {
        return progress_ >= 1.0f;
    }

    return true;
}

float SplashScreen::ElapsedSeconds() const noexcept
{
    const auto now = std::chrono::steady_clock::now();
    const std::chrono::duration<float> elapsed = now - start_time_;
    return elapsed.count();
}

void SplashScreen::Render(ImFont* title_font,
                          ImFont* heading_font,
                          ImFont* regular_font,
                          ImFont* small_font,
                          const ellindyer::ui::branding::LogoTexture& logo)
{
    const ImGuiIO& io = ImGui::GetIO();

    const float window_x = (io.DisplaySize.x - kWindowWidth) * 0.5f;
    const float window_y = (io.DisplaySize.y - kWindowHeight) * 0.5f;

    ImGui::SetNextWindowPos(ImVec2(window_x, window_y));
    ImGui::SetNextWindowSize(ImVec2(kWindowWidth, kWindowHeight));

    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(28.0f, 24.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.05f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Border,   ImVec4(0.24f, 0.24f, 0.24f, 1.00f));

    ImGui::Begin(kSplashWindowName, nullptr, flags);

    const float content_width = ImGui::GetContentRegionAvail().x;

    if (logo.IsLoaded())
    {
        const auto& texture = logo.GetTexture();
        float logo_width  = static_cast<float>(texture.width);
        float logo_height = static_cast<float>(texture.height);
        if (logo_width > kLogoWidthMax || logo_height > kLogoHeightMax)
        {
            const float scale_x = kLogoWidthMax  / logo_width;
            const float scale_y = kLogoHeightMax / logo_height;
            const float scale   = scale_x < scale_y ? scale_x : scale_y;
            logo_width  *= scale;
            logo_height *= scale;
        }

        const float logo_x = ImGui::GetCursorPosX()
                           + (content_width - logo_width) * 0.5f;
        ImGui::SetCursorPosX(logo_x);

        ImGui::Image(reinterpret_cast<ImTextureID>(texture.view),
                     ImVec2(logo_width, logo_height));
    }
    else
    {
        const char* placeholder = "[ logo ]";
        RenderCenteredText(heading_font, placeholder, content_width, ColorTextMuted());
    }

    ImGui::Spacing();
    ImGui::Spacing();

    RenderCenteredText(title_font, application_name_, content_width, ColorTextPrimary());

    ImGui::Spacing();

    RenderCenteredText(regular_font, application_tagline_, content_width, ColorTextSecondary());

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    if (configuration_.show_status_text)
    {
        RenderCenteredText(small_font, status_message_, content_width, ColorTextSecondary());
    }

    if (configuration_.show_progress)
    {
        ImGui::Spacing();
        ImGui::Spacing();

        const float total_width = content_width;
        const float bar_height  = 4.0f;

        const ImVec2 bar_min(ImGui::GetCursorScreenPos());
        const ImVec2 bar_max(bar_min.x + total_width, bar_min.y + bar_height);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(bar_min, bar_max, ImGui::GetColorU32(ColorDim()));

        const float filled_width = total_width * std::clamp(progress_, 0.0f, 1.0f);
        const ImVec2 filled_max(bar_min.x + filled_width, bar_max.y);
        if (filled_width > 0.0f)
        {
            draw_list->AddRectFilled(bar_min, filled_max, ImGui::GetColorU32(ColorAccent()));
        }

        ImGui::Dummy(ImVec2(total_width, bar_height));

        ImGui::Spacing();
        RenderCenteredText(small_font,
                           std::to_string(static_cast<int>(progress_ * 100.0f)) + "%",
                           content_width,
                           ColorTextMuted());
    }

    ImGui::Spacing();

    RenderCenteredText(small_font, application_version_, content_width, ColorTextMuted());

    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
}

} // namespace ellindyer::ui::splash
