#pragma once

#include <cstdint>

#include <imgui.h>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Panels/InspectorPanel.hpp"
#include "UI/Panels/ProjectExplorerPanel.hpp"
#include "UI/Panels/StatusBarPanel.hpp"
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

    void Render(const ellindyer::ui::fonts::FontSet& fonts, float top_offset);

    [[nodiscard]] ellindyer::ui::panels::ProjectExplorerPanel& GetProjectExplorer() noexcept;
    [[nodiscard]] ellindyer::ui::panels::WorkspacePanel& GetWorkspace() noexcept;
    [[nodiscard]] ellindyer::ui::panels::InspectorPanel& GetInspector() noexcept;
    [[nodiscard]] ellindyer::ui::panels::StatusBarPanel& GetStatusBar() noexcept;

    void SetProjectExplorerVisible(bool visible) noexcept;
    void SetWorkspaceVisible(bool visible) noexcept;
    void SetInspectorVisible(bool visible) noexcept;
    void SetStatusBarVisible(bool visible) noexcept;

    [[nodiscard]] bool IsProjectExplorerVisible() const noexcept;
    [[nodiscard]] bool IsWorkspaceVisible() const noexcept;
    [[nodiscard]] bool IsInspectorVisible() const noexcept;
    [[nodiscard]] bool IsStatusBarVisible() const noexcept;

    void ResetVisibility() noexcept;
    void ResetLayout() noexcept;

private:
    void EnsureDefaultLayout(ImGuiID dockspace_id, ImVec2 size);

    ellindyer::ui::panels::ProjectExplorerPanel project_explorer_;
    ellindyer::ui::panels::WorkspacePanel       workspace_;
    ellindyer::ui::panels::InspectorPanel       inspector_;
    ellindyer::ui::panels::StatusBarPanel       status_bar_;

    bool project_explorer_visible_ = true;
    bool workspace_visible_        = true;
    bool inspector_visible_        = true;
    bool status_bar_visible_       = true;

    bool layout_initialized_ = false;
};

} // namespace ellindyer::ui::shell
