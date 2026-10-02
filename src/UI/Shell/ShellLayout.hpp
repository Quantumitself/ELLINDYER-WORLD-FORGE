#pragma once

#include <memory>

#include <imgui.h>

#include "UI/Docking/DockSpace.hpp"
#include "UI/Fonts/FontManager.hpp"
#include "UI/Panels/InspectorPanel.hpp"
#include "UI/Panels/ProjectExplorerPanel.hpp"
#include "UI/Panels/WorkspacePanel.hpp"
#include "UI/StatusBar/StatusBar.hpp"

namespace ellindyer::ui::shell
{

struct ShellLayoutConfiguration
{
    ellindyer::ui::docking::DockSpaceLayout dockspace{};
};

class ShellLayout
{
public:
    ShellLayout();
    ~ShellLayout();

    ShellLayout(const ShellLayout&) = delete;
    ShellLayout& operator=(const ShellLayout&) = delete;
    ShellLayout(ShellLayout&&) noexcept = delete;
    ShellLayout& operator=(ShellLayout&&) noexcept = delete;

    void Configure(const ShellLayoutConfiguration& configuration);

    void Render(const ellindyer::ui::fonts::FontSet& fonts,
                float top_offset,
                float bottom_reserved);

    [[nodiscard]] ellindyer::ui::panels::ProjectExplorerPanel& GetProjectExplorer() noexcept;
    [[nodiscard]] ellindyer::ui::panels::WorkspacePanel& GetWorkspace() noexcept;
    [[nodiscard]] ellindyer::ui::panels::InspectorPanel& GetInspector() noexcept;
    [[nodiscard]] ellindyer::ui::docking::DockSpace& GetDockSpace() noexcept;

    void SetProjectExplorerVisible(bool visible) noexcept;
    void SetWorkspaceVisible(bool visible) noexcept;
    void SetInspectorVisible(bool visible) noexcept;

    [[nodiscard]] bool IsProjectExplorerVisible() const noexcept;
    [[nodiscard]] bool IsWorkspaceVisible() const noexcept;
    [[nodiscard]] bool IsInspectorVisible() const noexcept;

    void ResetVisibility() noexcept;
    void ResetSplitters() noexcept;

private:
    void RenderBody(const ellindyer::ui::fonts::FontSet& fonts,
                    float top_offset,
                    float bottom_reserved);

    void InitializePanels();

    ShellLayoutConfiguration                     configuration_{};
    ellindyer::ui::docking::DockSpace            dock_space_;
    ellindyer::ui::panels::ProjectExplorerPanel  project_explorer_;
    ellindyer::ui::panels::WorkspacePanel        workspace_;
    ellindyer::ui::panels::InspectorPanel        inspector_;

    std::shared_ptr<ellindyer::ui::docking::DockPanel> project_explorer_dock_;
    std::shared_ptr<ellindyer::ui::docking::DockPanel> workspace_dock_;
    std::shared_ptr<ellindyer::ui::docking::DockPanel> inspector_dock_;

    ellindyer::ui::fonts::FontSet                cached_fonts_{};
    bool                                         panels_initialized_ = false;
};

} // namespace ellindyer::ui::shell
