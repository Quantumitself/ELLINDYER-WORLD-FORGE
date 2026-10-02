#include "UI/Shell/ShellLayout.hpp"

#include <algorithm>

#include <imgui.h>
#include <imgui_internal.h>

namespace ellindyer::ui::shell
{

ShellLayout::ShellLayout() = default;
ShellLayout::~ShellLayout() = default;

void ShellLayout::Configure(const ShellLayoutConfiguration& configuration)
{
    configuration_ = configuration;
    dock_space_.SetLayout(configuration_.dockspace);
    InitializePanels();
}

void ShellLayout::Render(const ellindyer::ui::fonts::FontSet& fonts,
                         float top_offset,
                         float bottom_reserved)
{
    cached_fonts_ = fonts;
    RenderBody(fonts, top_offset, bottom_reserved);
}

void ShellLayout::RenderBody(const ellindyer::ui::fonts::FontSet& fonts,
                             float top_offset,
                             float bottom_reserved)
{
    (void)fonts;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    const float host_y      = viewport->Pos.y + top_offset;
    const float host_height = viewport->Size.y - top_offset - bottom_reserved;

    if (host_height <= 0.0f)
    {
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, host_y));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, host_height));
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr ImGuiWindowFlags host_flags =
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("##WorldForgeDockHost", nullptr, host_flags);

    dock_space_.Render(fonts);

    ImGui::End();
    ImGui::PopStyleVar(3);
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

ellindyer::ui::docking::DockSpace& ShellLayout::GetDockSpace() noexcept
{
    return dock_space_;
}

void ShellLayout::SetProjectExplorerVisible(bool visible) noexcept
{
    dock_space_.SetZoneVisible(ellindyer::ui::docking::DockZone::Left, visible);
}

void ShellLayout::SetWorkspaceVisible(bool visible) noexcept
{
    dock_space_.SetZoneVisible(ellindyer::ui::docking::DockZone::Center, visible);
}

void ShellLayout::SetInspectorVisible(bool visible) noexcept
{
    dock_space_.SetZoneVisible(ellindyer::ui::docking::DockZone::Right, visible);
}

bool ShellLayout::IsProjectExplorerVisible() const noexcept
{
    return dock_space_.IsZoneVisible(ellindyer::ui::docking::DockZone::Left);
}

bool ShellLayout::IsWorkspaceVisible() const noexcept
{
    return dock_space_.IsZoneVisible(ellindyer::ui::docking::DockZone::Center);
}

bool ShellLayout::IsInspectorVisible() const noexcept
{
    return dock_space_.IsZoneVisible(ellindyer::ui::docking::DockZone::Right);
}

void ShellLayout::ResetVisibility() noexcept
{
    dock_space_.SetZoneVisible(ellindyer::ui::docking::DockZone::Left, true);
    dock_space_.SetZoneVisible(ellindyer::ui::docking::DockZone::Center, true);
    dock_space_.SetZoneVisible(ellindyer::ui::docking::DockZone::Right, true);
}

void ShellLayout::ResetSplitters() noexcept
{
    dock_space_.ResetSplitters();
}

void ShellLayout::InitializePanels()
{
    using ellindyer::ui::docking::DockPanel;
    using ellindyer::ui::docking::DockZone;

    dock_space_.ClearPanels();

    project_explorer_dock_ = std::make_shared<DockPanel>("ProjectExplorer", "Project Explorer");
    project_explorer_dock_->SetMinWidth(220.0f);
    project_explorer_dock_->SetContentRenderer(
        [this]()
        {
            ellindyer::ui::layout::PanelLayoutMetrics metrics{};
            metrics.left_width   = dock_space_.GetLeftWidth();
            metrics.center_width = dock_space_.GetCenterWidth();
            metrics.right_width  = dock_space_.GetRightWidth();
            metrics.panel_height = dock_space_.GetCenterHeight() + dock_space_.GetBottomHeight();
            project_explorer_.Render(cached_fonts_, metrics);
        });
    dock_space_.AddPanel(DockZone::Left, project_explorer_dock_);

    workspace_dock_ = std::make_shared<DockPanel>("Workspace", "Workspace");
    workspace_dock_->SetMinWidth(320.0f);
    workspace_dock_->SetMinHeight(240.0f);
    workspace_dock_->SetContentRenderer(
        [this]()
        {
            ellindyer::ui::layout::PanelLayoutMetrics metrics{};
            metrics.left_width   = dock_space_.GetLeftWidth();
            metrics.center_width = dock_space_.GetCenterWidth();
            metrics.right_width  = dock_space_.GetRightWidth();
            metrics.panel_height = dock_space_.GetCenterHeight();
            workspace_.Render(cached_fonts_, metrics);
        });
    dock_space_.AddPanel(DockZone::Center, workspace_dock_);

    inspector_dock_ = std::make_shared<DockPanel>("Inspector", "Inspector");
    inspector_dock_->SetMinWidth(260.0f);
    inspector_dock_->SetContentRenderer(
        [this]()
        {
            ellindyer::ui::layout::PanelLayoutMetrics metrics{};
            metrics.left_width   = dock_space_.GetLeftWidth();
            metrics.center_width = dock_space_.GetCenterWidth();
            metrics.right_width  = dock_space_.GetRightWidth();
            metrics.panel_height = dock_space_.GetCenterHeight() + dock_space_.GetBottomHeight();
            inspector_.Render(cached_fonts_, metrics);
        });
    dock_space_.AddPanel(DockZone::Right, inspector_dock_);

    panels_initialized_ = true;
}

} // namespace ellindyer::ui::shell
