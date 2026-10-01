#pragma once

#include <string>
#include <vector>

#include "UI/Fonts/FontManager.hpp"

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

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

    void SetProjectName(std::string name);
    void AddRootEntry(std::string entry);
    void Clear();

private:
    std::string              project_name_;
    std::vector<std::string> root_entries_;
};

} // namespace ellindyer::ui::panels
