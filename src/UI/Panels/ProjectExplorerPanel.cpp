#include "UI/Panels/ProjectExplorerPanel.hpp"

#include <imgui.h>

namespace ellindyer::ui::panels
{

namespace
{

const char* kDefaultProjectLabel = "No project loaded";

} // namespace

ProjectExplorerPanel::ProjectExplorerPanel() = default;

ProjectExplorerPanel::~ProjectExplorerPanel() = default;

void ProjectExplorerPanel::Render(
    const ellindyer::ui::fonts::FontSet& fonts,
    const ellindyer::ui::layout::PanelLayoutMetrics& metrics)
{
    using ellindyer::ui::layout::Panel;
    using ellindyer::ui::layout::PanelStyle;

    Panel panel("ProjectExplorer", "Project Explorer");

    PanelStyle style{};
    panel.SetStyle(style);
    panel.SetMinWidth(metrics.left_width);

    panel.BeginPanel(metrics.left_width, metrics.panel_height);

    panel.RenderHeader(fonts);

    if (project_name_.empty())
    {
        panel.RenderTextMuted(fonts, kDefaultProjectLabel);
        panel.RenderTextMuted(fonts, "Open or create a project to populate the tree.");
    }
    else
    {
        panel.RenderText(fonts, project_name_);
        panel.RenderSeparator();

        if (root_entries_.empty())
        {
            panel.RenderTextMuted(fonts, "(empty)");
        }
        else
        {
            for (const std::string& entry : root_entries_)
            {
                panel.RenderText(fonts, entry);
            }
        }
    }

    panel.EndPanel();
}

void ProjectExplorerPanel::SetProjectName(std::string name)
{
    project_name_ = std::move(name);
}

void ProjectExplorerPanel::AddRootEntry(std::string entry)
{
    if (entry.empty())
    {
        return;
    }
    root_entries_.push_back(std::move(entry));
}

void ProjectExplorerPanel::Clear()
{
    project_name_.clear();
    root_entries_.clear();
}

} // namespace ellindyer::ui::panels
