#include "UI/Shell/ShellLayout.hpp"

#include <algorithm>

namespace ellindyer::ui::shell
{

namespace
{

constexpr float kStatusBarHeight    = 26.0f;
constexpr float kStatusBarSpacing   = 4.0f;

} // namespace

ShellLayout::ShellLayout() = default;

ShellLayout::~ShellLayout() = default;

void ShellLayout::Configure(const ShellLayoutConfiguration& configuration)
{
    configuration_ = configuration;
    panel_layout_.Configure(configuration_.panels);
}

void ShellLayout::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    RenderBody(fonts);
    RenderStatusBar(fonts);
}

ellindyer::ui::panels::ProjectExplorerPanel& ShellLayout::GetProjectExplorer() noexcept
{
    return project_explorer_;
}

ellindyer::ui::panels::WorkspacePanel& ShellLayout::GetWorkspace() noexcept
{
    return workspace_;
}

ellindyer::ui::panels::InspectorPanel& ShellLayout::GetInspector() noexcept
{
    return inspector_;
}

ellindyer::ui::panels::StatusBarPanel& ShellLayout::GetStatusBar() noexcept
{
    return status_bar_;
}

void ShellLayout::SetProjectExplorerVisible(bool visible) noexcept
{
    project_explorer_visible_ = visible;
}

void ShellLayout::SetWorkspaceVisible(bool visible) noexcept
{
    workspace_visible_ = visible;
}

void ShellLayout::SetInspectorVisible(bool visible) noexcept
{
    inspector_visible_ = visible;
}

void ShellLayout::SetStatusBarVisible(bool visible) noexcept
{
    status_bar_visible_ = visible;
}

bool ShellLayout::IsProjectExplorerVisible() const noexcept
{
    return project_explorer_visible_;
}

bool ShellLayout::IsWorkspaceVisible() const noexcept
{
    return workspace_visible_;
}

bool ShellLayout::IsInspectorVisible() const noexcept
{
    return inspector_visible_;
}

bool ShellLayout::IsStatusBarVisible() const noexcept
{
    return status_bar_visible_;
}

void ShellLayout::ResetVisibility() noexcept
{
    project_explorer_visible_ = true;
    workspace_visible_        = true;
    inspector_visible_        = true;
    status_bar_visible_       = true;
}

void ShellLayout::RenderBody(const ellindyer::ui::fonts::FontSet& fonts)
{
    ImVec2 available = ImGui::GetContentRegionAvail();

    if (status_bar_visible_)
    {
        available.y -= kStatusBarHeight + kStatusBarSpacing;
        if (available.y < 0.0f)
        {
            available.y = 0.0f;
        }
    }

    const bool any_visible =
        project_explorer_visible_ || workspace_visible_ || inspector_visible_;

    if (!any_visible)
    {
        return;
    }

    ellindyer::ui::layout::PanelLayoutConfiguration effective_configuration =
        panel_layout_.GetConfiguration();

    if (!project_explorer_visible_)
    {
        effective_configuration.left_width_fraction  = 0.0f;
        effective_configuration.left_min_width       = 0.0f;
    }
    if (!inspector_visible_)
    {
        effective_configuration.right_width_fraction = 0.0f;
        effective_configuration.right_min_width      = 0.0f;
    }

    const int visible_side_panels =
        (project_explorer_visible_ ? 1 : 0) + (inspector_visible_ ? 1 : 0);

    const float total_spacing = effective_configuration.panel_spacing *
                                static_cast<float>(visible_side_panels +
                                                   (workspace_visible_ ? 1 : 0) - 1);
    ImVec2 adjusted_available = available;
    adjusted_available.x -= (effective_configuration.panel_spacing *
                             static_cast<float>(2 - visible_side_panels));
    if (adjusted_available.x < 0.0f)
    {
        adjusted_available.x = 0.0f;
    }

    ellindyer::ui::layout::PanelLayout effective_layout;
    effective_layout.Configure(effective_configuration);
    const ellindyer::ui::layout::PanelLayoutMetrics metrics =
        effective_layout.ComputeMetrics(adjusted_available);

    const float spacing = effective_configuration.panel_spacing;

    bool first_rendered = false;

    if (project_explorer_visible_)
    {
        project_explorer_.Render(fonts, metrics);
        first_rendered = true;
    }

    if (workspace_visible_)
    {
        if (first_rendered)
        {
            ImGui::SameLine(0.0f, spacing);
        }
        workspace_.Render(fonts, metrics);
        first_rendered = true;
    }

    if (inspector_visible_)
    {
        if (first_rendered)
        {
            ImGui::SameLine(0.0f, spacing);
        }
        inspector_.Render(fonts, metrics);
    }

    (void)total_spacing;
}

void ShellLayout::RenderStatusBar(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (!status_bar_visible_)
    {
        return;
    }

    ImGui::Spacing();
    status_bar_.Render(fonts);
}

} // namespace ellindyer::ui::shell
