#include "UI/Panels/WorkspacePanel.hpp"

#include <imgui.h>

#include "UI/ImGui/ImGuiCompat.hpp"

namespace ellindyer::ui::panels
{

namespace
{
ImVec4 ColorPrimary() { return ImVec4(1.00f, 1.00f, 1.00f, 1.00f); }
ImVec4 ColorMuted()   { return ImVec4(0.45f, 0.45f, 0.45f, 1.00f); }
} // namespace

WorkspacePanel::WorkspacePanel() = default;
WorkspacePanel::~WorkspacePanel() = default;

void WorkspacePanel::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (!ImGui::Begin("Workspace"))
    {
        ImGui::End();
        return;
    }

    if (!description_.empty())
    {
        if (fonts.large_regular != nullptr)
        {
            ImGui::PushFont(fonts.large_regular, fonts.large_regular->LegacySize);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPrimary());
        ImGui::TextUnformatted(description_.c_str());
        ImGui::PopStyleColor();
        if (fonts.large_regular != nullptr)
        {
            ImGui::PopFont();
        }
        ImGui::Spacing();
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
    for (const std::string& hint : hints_)
    {
        ImGui::TextUnformatted(hint.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::End();
}

void WorkspacePanel::SetTitle(std::string title) { title_ = std::move(title); }
void WorkspacePanel::SetDescription(std::string description) { description_ = std::move(description); }

void WorkspacePanel::AddHint(std::string hint)
{
    if (hint.empty()) return;
    hints_.push_back(std::move(hint));
}

void WorkspacePanel::Clear()
{
    description_.clear();
    hints_.clear();
}

} // namespace ellindyer::ui::panels
