#include "UI/Panels/InspectorPanel.hpp"

#include <imgui.h>

#include "UI/ImGui/ImGuiCompat.hpp"

namespace ellindyer::ui::panels
{

namespace
{
ImVec4 ColorPrimary() { return ImVec4(1.00f, 1.00f, 1.00f, 1.00f); }
ImVec4 ColorMuted()   { return ImVec4(0.45f, 0.45f, 0.45f, 1.00f); }
} // namespace

InspectorPanel::InspectorPanel() = default;
InspectorPanel::~InspectorPanel() = default;

void InspectorPanel::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    
    (void)fonts;
if (!ImGui::Begin("Inspector"))
    {
        ImGui::End();
        return;
    }

    if (selection_title_.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
        ImGui::TextUnformatted("Nothing selected.");
        ImGui::TextUnformatted("Select an object to inspect its properties.");
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ColorPrimary());
        ImGui::TextUnformatted(selection_title_.c_str());
        ImGui::PopStyleColor();

        if (!selection_subtitle_.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
            ImGui::TextUnformatted(selection_subtitle_.c_str());
            ImGui::PopStyleColor();
        }

        ImGui::Separator();
        ImGui::Spacing();

        if (properties_.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
            ImGui::TextUnformatted("(no properties)");
            ImGui::PopStyleColor();
        }
        else
        {
            for (const PropertyEntry& entry : properties_)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
                ImGui::TextUnformatted(entry.key.c_str());
                ImGui::PopStyleColor();

                ImGui::PushStyleColor(ImGuiCol_Text, ColorPrimary());
                ImGui::TextUnformatted(("  " + entry.value).c_str());
                ImGui::PopStyleColor();
            }
        }
    }

    ImGui::End();
}

void InspectorPanel::SetSelectionTitle(std::string title) { selection_title_ = std::move(title); }
void InspectorPanel::SetSelectionSubtitle(std::string subtitle) { selection_subtitle_ = std::move(subtitle); }

void InspectorPanel::AddSection(std::string section_title)
{
    if (section_title.empty()) return;
    sections_.push_back(std::move(section_title));
}

void InspectorPanel::AddProperty(std::string key, std::string value)
{
    if (key.empty()) return;
    properties_.push_back(PropertyEntry{std::move(key), std::move(value)});
}

void InspectorPanel::Clear()
{
    selection_title_.clear();
    selection_subtitle_.clear();
    sections_.clear();
    properties_.clear();
}

} // namespace ellindyer::ui::panels
