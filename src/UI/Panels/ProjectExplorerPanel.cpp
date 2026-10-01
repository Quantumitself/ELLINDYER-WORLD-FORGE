#include "UI/Panels/ProjectExplorerPanel.hpp"

#include <imgui.h>

#include "UI/ImGui/ImGuiCompat.hpp"

namespace ellindyer::ui::panels
{

namespace
{
const char* kDefaultProjectLabel = "No project loaded";
ImVec4 ColorMuted()   { return ImVec4(0.45f, 0.45f, 0.45f, 1.00f); }
ImVec4 ColorPrimary() { return ImVec4(1.00f, 1.00f, 1.00f, 1.00f); }
} // namespace

ProjectExplorerPanel::ProjectExplorerPanel() = default;
ProjectExplorerPanel::~ProjectExplorerPanel() = default;

void ProjectExplorerPanel::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (!ImGui::Begin("Project Explorer"))
    {
        ImGui::End();
        return;
    }

    if (project_name_.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
        ImGui::TextUnformatted(kDefaultProjectLabel);
        ImGui::TextUnformatted("Open or create a project to populate the tree.");
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPrimary());
        ImGui::TextUnformatted(project_name_.c_str());
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::Spacing();

        if (root_entries_.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
            ImGui::TextUnformatted("(empty)");
            ImGui::PopStyleColor();
        }
        else
        {
            for (const std::string& entry : root_entries_)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
                ImGui::TextUnformatted(entry.c_str());
                ImGui::PopStyleColor();
            }
        }
    }

    ImGui::End();
}

void ProjectExplorerPanel::SetProjectName(std::string name) { project_name_ = std::move(name); }

void ProjectExplorerPanel::AddRootEntry(std::string entry)
{
    if (entry.empty()) return;
    root_entries_.push_back(std::move(entry));
}

void ProjectExplorerPanel::Clear()
{
    project_name_.clear();
    root_entries_.clear();
}

} // namespace ellindyer::ui::panels
