#pragma once

#include <string>
#include <vector>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Layout/Panel.hpp"
#include "UI/Layout/PanelLayout.hpp"

namespace ellindyer::ui::panels
{

class WorkspacePanel
{
public:
    WorkspacePanel();
    ~WorkspacePanel();

    WorkspacePanel(const WorkspacePanel&) = delete;
    WorkspacePanel& operator=(const WorkspacePanel&) = delete;
    WorkspacePanel(WorkspacePanel&&) noexcept = delete;
    WorkspacePanel& operator=(WorkspacePanel&&) noexcept = delete;

    void Render(const ellindyer::ui::fonts::FontSet& fonts,
                const ellindyer::ui::layout::PanelLayoutMetrics& metrics);

    void SetTitle(std::string title);

    void SetDescription(std::string description);

    void AddHint(std::string hint);

    void Clear();

private:
    std::string              title_       = "Workspace";
    std::string              description_;
    std::vector<std::string> hints_;
};

} // namespace ellindyer::ui::panels
