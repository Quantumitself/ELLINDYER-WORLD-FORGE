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

void ShellLayout::RenderBody(const ellindyer::ui::fonts::FontSet& fonts)
{
    ImVec2 available = ImGui::GetContentRegionAvail();

    available.y -= kStatusBarHeight + kStatusBarSpacing;
    if (available.y < 0.0f)
    {
        available.y = 0.0f;
    }

    const ellindyer::ui::layout::PanelLayoutMetrics metrics =
        panel_layout_.ComputeMetrics(available);

    const float spacing = metrics.spacing;

    project_explorer_.Render(fonts, metrics);

    ImGui::SameLine(0.0f, spacing);

    workspace_.Render(fonts, metrics);

    ImGui::SameLine(0.0f, spacing);

    inspector_.Render(fonts, metrics);
}

void ShellLayout::RenderStatusBar(const ellindyer::ui::fonts::FontSet& fonts)
{
    ImGui::Spacing();
    status_bar_.Render(fonts);
}

} // namespace ellindyer::ui::shell
