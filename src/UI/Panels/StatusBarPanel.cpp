#include "UI/Panels/StatusBarPanel.hpp"

#include <imgui.h>

#include "UI/ImGui/ImGuiCompat.hpp"

namespace ellindyer::ui::panels
{

namespace
{
ImVec4 ColorMuted() { return ImVec4(0.45f, 0.45f, 0.45f, 1.00f); }
} // namespace

StatusBarPanel::StatusBarPanel() = default;
StatusBarPanel::~StatusBarPanel() = default;

void StatusBarPanel::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (!ImGui::Begin("Status"))
    {
        ImGui::End();
        return;
    }

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
        ImGui::SameLine(); ImGui::TextUnformatted(" | "); ImGui::SameLine();
        ImGui::TextUnformatted(middle_text_.c_str());
    }
    if (!right_text_.empty())
    {
        ImGui::SameLine(); ImGui::TextUnformatted(" | "); ImGui::SameLine();
        ImGui::TextUnformatted(right_text_.c_str());
    }

    ImGui::PopStyleColor();
    if (fonts.small_regular != nullptr)
    {
        ImGui::PopFont();
    }

    ImGui::End();
}

void StatusBarPanel::SetLeftText(std::string text) { left_text_ = std::move(text); }
void StatusBarPanel::SetMiddleText(std::string text) { middle_text_ = std::move(text); }
void StatusBarPanel::SetRightText(std::string text) { right_text_ = std::move(text); }

} // namespace ellindyer::ui::panels
