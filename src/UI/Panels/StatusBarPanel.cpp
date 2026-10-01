#include "UI/Panels/StatusBarPanel.hpp"

#include <imgui.h>

namespace ellindyer::ui::panels
{

namespace
{

constexpr float kStatusBarHeight = 26.0f;

constexpr ImGuiWindowFlags kStatusBarFlags =
    ImGuiWindowFlags_NoScrollbar
    | ImGuiWindowFlags_NoScrollWithMouse
    | ImGuiWindowFlags_NoBackground;

ImVec4 ColorMuted() { return ImVec4(0.45f, 0.45f, 0.45f, 1.00f); }

} // namespace

StatusBarPanel::StatusBarPanel() = default;

StatusBarPanel::~StatusBarPanel() = default;

void StatusBarPanel::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 4.0f));

    ImGui::BeginChild("##StatusBarPanel",
                      ImVec2(0.0f, kStatusBarHeight),
                      false,
                      kStatusBarFlags);

    if (fonts.small_regular != nullptr)
    {
        ImGui::PushFont(fonts.small_regular, fonts.small_regular->LegacySize);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());

    if (!left_text_.empty())
    {
        ImGui::TextUnformatted(left_text_.c_str());
    }

    if (!middle_text_.empty())
    {
        ImGui::SameLine();
        ImGui::TextUnformatted(" | ");
        ImGui::SameLine();
        ImGui::TextUnformatted(middle_text_.c_str());
    }

    if (!right_text_.empty())
    {
        const float right_width = ImGui::CalcTextSize(right_text_.c_str()).x;
        const float available   = ImGui::GetContentRegionAvail().x;
        const float cursor_x    = ImGui::GetCursorPosX() + available - right_width;
        if (cursor_x > ImGui::GetCursorPosX())
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(cursor_x);
            ImGui::TextUnformatted(right_text_.c_str());
        }
    }

    ImGui::PopStyleColor();

    if (fonts.small_regular != nullptr)
    {
        ImGui::PopFont();
    }

    ImGui::EndChild();

    ImGui::PopStyleVar();
}

void StatusBarPanel::SetLeftText(std::string text)
{
    left_text_ = std::move(text);
}

void StatusBarPanel::SetMiddleText(std::string text)
{
    middle_text_ = std::move(text);
}

void StatusBarPanel::SetRightText(std::string text)
{
    right_text_ = std::move(text);
}

} // namespace ellindyer::ui::panels
