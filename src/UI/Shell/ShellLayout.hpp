#pragma once

#include <imgui.h>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Panels/InspectorPanel.hpp"
#include "UI/Panels/ProjectExplorerPanel.hpp"
#include "UI/Panels/WorkspacePanel.hpp"

namespace ellindyer::ui::shell
{

class ShellLayout
{
public:
    ShellLayout();
    ~ShellLayout();

    ShellLayout(const ShellLayout&) = delete;
    ShellLayout& operator=(const ShellLayout&) = delete;
    ShellLayout(ShellLayout&&) noexcept = delete;
    ShellLayout& operator=(ShellLayout&&) noexcept = delete;

    void Configure();

    // top_offset: pixels from top of viewport where the DockSpace begins
    // bottom_reserved: pixels reserved at bottom of viewport for status bar
    void Render(const ellindyer::ui::fonts::FontSet& fonts,
                float top_offset,
                float bottom_reserved);

    [[nodiscard]] ellindyer::ui::panels::ProjectExplorerPanel& GetProjectExplorer() noexcept;
    [[nodiscard]] ellindyer::ui::panels::WorkspacePanel& GetWorkspace() noexcept;
    [[nodiscard]] ellindyer::ui::panels::InspectorPanel& GetInspector() noexcept;

    void SetProjectExplorerVisible(bool visible) noexcept;
    void SetWorkspaceVisible(bool visible) noexcept;
    void SetInspectorVisible(bool visible) noexcept;

    [[nodiscard]] bool IsProjectExplorerVisible() const noexcept;
    [[nodiscard]] bool IsWorkspaceVisible() const noexcept;
    [[nodiscard]] bool IsInspectorVisible() const noexcept;

    void ResetVisibility() noexcept;
    void ResetLayout() noexcept;

private:
    void EnsureDefaultLayout(ImGuiID dockspace_id, ImVec2 size);

    ellindyer::ui::panels::ProjectExplorerPanel project_explorer_;
    ellindyer::ui::panels::WorkspacePanel       workspace_;
    ellindyer::ui::panels::InspectorPanel       inspector_;

    bool project_explorer_visible_ = true;
    bool workspace_visible_        = true;
    bool inspector_visible_        = true;

    bool layout_initialized_ = false;
};

} // namespace ellindyer::ui::shell
