#pragma once

#include <cstdint>

#include <imgui.h>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Layout/PanelLayout.hpp"
#include "UI/Panels/InspectorPanel.hpp"
#include "UI/Panels/ProjectExplorerPanel.hpp"
#include "UI/Panels/StatusBarPanel.hpp"
#include "UI/Panels/WorkspacePanel.hpp"

namespace ellindyer::ui::shell
{

struct ShellLayoutConfiguration
{
    ellindyer::ui::layout::PanelLayoutConfiguration panels{};
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

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

    [[nodiscard]] ellindyer::ui::panels::ProjectExplorerPanel& GetProjectExplorer() noexcept;

    [[nodiscard]] ellindyer::ui::panels::WorkspacePanel& GetWorkspace() noexcept;

    [[nodiscard]] ellindyer::ui::panels::InspectorPanel& GetInspector() noexcept;

    [[nodiscard]] ellindyer::ui::panels::StatusBarPanel& GetStatusBar() noexcept;

private:
    void RenderBody(const ellindyer::ui::fonts::FontSet& fonts);

    void RenderStatusBar(const ellindyer::ui::fonts::FontSet& fonts);

    ShellLayoutConfiguration                         configuration_{};
    ellindyer::ui::layout::PanelLayout               panel_layout_{};
    ellindyer::ui::panels::ProjectExplorerPanel      project_explorer_;
    ellindyer::ui::panels::WorkspacePanel            workspace_;
    ellindyer::ui::panels::InspectorPanel            inspector_;
    ellindyer::ui::panels::StatusBarPanel            status_bar_;
};

} // namespace ellindyer::ui::shell
