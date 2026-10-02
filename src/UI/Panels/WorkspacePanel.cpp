#include "UI/Panels/WorkspacePanel.hpp"

#include <imgui.h>

#include "UI/ImGui/ImGuiCompat.hpp"

namespace ellindyer::ui::panels
{

WorkspacePanel::WorkspacePanel() = default;

WorkspacePanel::~WorkspacePanel() = default;

void WorkspacePanel::Render(
    const ellindyer::ui::fonts::FontSet& fonts,
    const ellindyer::ui::layout::PanelLayoutMetrics& metrics)
{
    using ellindyer::ui::layout::Panel;
    using ellindyer::ui::layout::PanelStyle;

    Panel panel("Workspace", title_);

    PanelStyle style{};
    style.padding_x = 14.0f;
    style.padding_y = 14.0f;
    panel.SetStyle(style);
    panel.SetMinWidth(metrics.center_width);

    panel.BeginPanel(metrics.center_width, metrics.panel_height);

    panel.RenderHeader(fonts);

    if (!description_.empty())
    {
        if (fonts.large_regular != nullptr)
        {
            ImGui::PushFont(fonts.large_regular, fonts.large_regular->LegacySize);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
        ImGui::TextUnformatted(description_.c_str());
        ImGui::PopStyleColor();

        if (fonts.large_regular != nullptr)
        {
            ImGui::PopFont();
        }

        ImGui::Spacing();
    }

    for (const std::string& hint : hints_)
    {
        panel.RenderTextMuted(fonts, hint);
    }

    panel.EndPanel();
}

void WorkspacePanel::SetTitle(std::string title)
{
    title_ = std::move(title);
}

void WorkspacePanel::SetDescription(std::string description)
{
    description_ = std::move(description);
}

void WorkspacePanel::AddHint(std::string hint)
{
    if (hint.empty())
    {
        return;
    }
    hints_.push_back(std::move(hint));
}

void WorkspacePanel::Clear()
{
    description_.clear();
    hints_.clear();
}

} // namespace ellindyer::ui::panels
