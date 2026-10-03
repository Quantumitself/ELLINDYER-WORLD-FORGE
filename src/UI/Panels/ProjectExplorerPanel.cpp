#include "UI/Panels/ProjectExplorerPanel.hpp"

#include <imgui.h>

#include <utility>

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

    if (has_tree_ && tree_.HasRoot())
    {
        tree_.Render(fonts);
    }
    else
    {
        RenderPlaceholder(fonts);
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
    tree_.Clear();
    has_tree_ = false;
}

const std::string& ProjectExplorerPanel::GetProjectName() const noexcept
{
    return project_name_;
}

const std::vector<std::string>& ProjectExplorerPanel::GetRootEntries() const noexcept
{
    return root_entries_;
}

void ProjectExplorerPanel::RebuildFromProject(const ellindyer::project::Project& project)
{
    ellindyer::ui::project_explorer::ExplorerBuildOptions options{};
    RebuildFromProject(project, options);
}

void ProjectExplorerPanel::RebuildFromProject(
    const ellindyer::project::Project& project,
    const ellindyer::ui::project_explorer::ExplorerBuildOptions& options)
{
    project_name_ = project.GetDisplayName();

    const ellindyer::ui::project_explorer::ExplorerNode root =
        ellindyer::ui::project_explorer::ExplorerNodeBuilder::BuildFromProject(
            project, options);

    tree_.SetRoot(root);
    tree_.ExpandPath(project.GetLayout().root);

    for (const auto& section : root.children)
    {
        if (section.kind == ellindyer::ui::project_explorer::ExplorerNodeKind::Section)
        {
            tree_.GetState().SetExpanded(section.id, false);
            if (!section.path.empty())
            {
                tree_.GetState().SetExpandedByPath(section.path, false);
            }
        }
    }

    has_tree_ = true;
}

void ProjectExplorerPanel::ClearTree()
{
    tree_.Clear();
    has_tree_ = false;
}

ellindyer::ui::project_explorer::ProjectExplorerTree&
ProjectExplorerPanel::GetTree() noexcept
{
    return tree_;
}

const ellindyer::ui::project_explorer::ProjectExplorerTree&
ProjectExplorerPanel::GetTree() const noexcept
{
    return tree_;
}

void ProjectExplorerPanel::SetNodeSelectedHandler(NodeSelectedHandler handler)
{
    node_selected_handler_ = std::move(handler);
    tree_.SetSelectionHandler(
        [this](const ellindyer::ui::project_explorer::ExplorerNode& node)
        {
            if (node_selected_handler_)
            {
                node_selected_handler_(node);
            }
        });
}

void ProjectExplorerPanel::RenderPlaceholder(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    if (project_name_.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.45f, 0.45f, 1.00f));
        ImGui::TextUnformatted(kDefaultProjectLabel);
        ImGui::TextUnformatted("Open or create a project to populate the tree.");
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::TextUnformatted(project_name_.c_str());

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.45f, 0.45f, 1.00f));
        ImGui::Separator();
        for (const std::string& entry : root_entries_)
        {
            ImGui::TextUnformatted(entry.c_str());
        }
        ImGui::PopStyleColor();
    }

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }
}

} // namespace ellindyer::ui::panels
