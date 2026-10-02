#include "UI/Shell/ShellLayout.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace ellindyer::ui::shell
{

namespace
{

constexpr const char* kDockSpaceName             = "WorldForgeDockSpace";
constexpr const char* kProjectExplorerWindowName = "Project Explorer";
constexpr const char* kWorkspaceWindowName       = "Workspace";
constexpr const char* kInspectorWindowName       = "Inspector";

} // namespace

ShellLayout::ShellLayout() = default;
ShellLayout::~ShellLayout() = default;

void ShellLayout::Configure()
{
}

void ShellLayout::Render(const ellindyer::ui::fonts::FontSet& fonts,
                         float top_offset,
                         float bottom_reserved)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    const float host_y = top_offset;
    const float host_height = viewport->Size.y - top_offset - bottom_reserved;

    if (host_height <= 0.0f)
    {
        return;
    }

    const ImVec2 host_pos(viewport->Pos.x, viewport->Pos.y + host_y);
    const ImVec2 host_size(viewport->Size.x, host_height);

    ImGui::SetNextWindowPos(host_pos);
    ImGui::SetNextWindowSize(host_size);
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

    const ImGuiID dockspace_id = ImGui::GetID(kDockSpaceName);
    EnsureDefaultLayout(dockspace_id, host_size);

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();
    ImGui::PopStyleVar(3);

    if (project_explorer_visible_)
    {
        project_explorer_.Render(fonts);
    }
    if (workspace_visible_)
    {
        workspace_.Render(fonts);
    }
    if (inspector_visible_)
    {
        inspector_.Render(fonts);
    }
}

void ShellLayout::EnsureDefaultLayout(ImGuiID dockspace_id, ImVec2 size)
{
    if (layout_initialized_)
    {
        return;
    }
    layout_initialized_ = true;

    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, size);

    ImGuiID center_id = dockspace_id;
    const ImGuiID left_id =
        ImGui::DockBuilderSplitNode(center_id, ImGuiDir_Left, 0.20f, nullptr, &center_id);
    const ImGuiID right_id =
        ImGui::DockBuilderSplitNode(center_id, ImGuiDir_Right, 0.22f, nullptr, &center_id);

    ImGui::DockBuilderDockWindow(kProjectExplorerWindowName, left_id);
    ImGui::DockBuilderDockWindow(kWorkspaceWindowName,       center_id);
    ImGui::DockBuilderDockWindow(kInspectorWindowName,       right_id);

    ImGui::DockBuilderFinish(dockspace_id);
}

ellindyer::ui::panels::ProjectExplorerPanel& ShellLayout::GetProjectExplorer() noexcept { return project_explorer_; }
ellindyer::ui::panels::WorkspacePanel& ShellLayout::GetWorkspace() noexcept { return workspace_; }
ellindyer::ui::panels::InspectorPanel& ShellLayout::GetInspector() noexcept { return inspector_; }

void ShellLayout::SetProjectExplorerVisible(bool visible) noexcept { project_explorer_visible_ = visible; }
void ShellLayout::SetWorkspaceVisible(bool visible) noexcept       { workspace_visible_ = visible; }
void ShellLayout::SetInspectorVisible(bool visible) noexcept       { inspector_visible_ = visible; }

bool ShellLayout::IsProjectExplorerVisible() const noexcept { return project_explorer_visible_; }
bool ShellLayout::IsWorkspaceVisible() const noexcept       { return workspace_visible_; }
bool ShellLayout::IsInspectorVisible() const noexcept       { return inspector_visible_; }

void ShellLayout::ResetVisibility() noexcept
{
    project_explorer_visible_ = true;
    workspace_visible_        = true;
    inspector_visible_        = true;
}

void ShellLayout::ResetLayout() noexcept
{
    layout_initialized_ = false;
    ResetVisibility();
}

} // namespace ellindyer::ui::shell
