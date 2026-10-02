#pragma once

#include <string>
#include <vector>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Layout/Panel.hpp"
#include <UI/Layout/PanelLayout.hpp>

namespace ellindyer::ui::panels
{

class ProjectExplorerPanel
{
public:
    ProjectExplorerPanel();
    ~ProjectExplorerPanel();

    ProjectExplorerPanel(const ProjectExplorerPanel&) = delete;
    ProjectExplorerPanel& operator=(const ProjectExplorerPanel&) = delete;
    ProjectExplorerPanel(ProjectExplorerPanel&&) noexcept = delete;
    ProjectExplorerPanel& operator=(ProjectExplorerPanel&&) noexcept = delete;

    void Render(const ellindyer::ui::fonts::FontSet& fonts,
                const ellindyer::ui::layout::PanelLayoutMetrics& metrics);

    void SetProjectName(std::string name);

    void AddRootEntry(std::string entry);

    void Clear();

    [[nodiscard]] const std::string& GetProjectName() const noexcept;

    [[nodiscard]] const std::vector<std::string>& GetRootEntries() const noexcept;

private:
    std::string               project_name_;
    std::vector<std::string>  root_entries_;
};

} // namespace ellindyer::ui::panels
