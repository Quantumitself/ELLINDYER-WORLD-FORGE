#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "Project/Project.hpp"
#include "UI/Fonts/FontManager.hpp"
#include "UI/Layout/Panel.hpp"
#include "UI/ProjectExplorer/ExplorerNodeBuilder.hpp"
#include "UI/ProjectExplorer/ProjectExplorerTree.hpp"
#include <UI/Layout/PanelLayout.hpp>

namespace ellindyer::ui::panels
{

class ProjectExplorerPanel
{
public:
    using NodeSelectedHandler =
        std::function<void(const ellindyer::ui::project_explorer::ExplorerNode&)>;

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

    void RebuildFromProject(const ellindyer::project::Project& project);

    void RebuildFromProject(const ellindyer::project::Project& project,
                            const ellindyer::ui::project_explorer::ExplorerBuildOptions& options);

    void ClearTree();

    [[nodiscard]] ellindyer::ui::project_explorer::ProjectExplorerTree& GetTree() noexcept;

    [[nodiscard]] const ellindyer::ui::project_explorer::ProjectExplorerTree&
        GetTree() const noexcept;

    void SetNodeSelectedHandler(NodeSelectedHandler handler);

private:
    void RenderPlaceholder(const ellindyer::ui::fonts::FontSet& fonts);

    std::string                                       project_name_;
    std::vector<std::string>                          root_entries_;
    ellindyer::ui::project_explorer::ProjectExplorerTree tree_;
    NodeSelectedHandler                               node_selected_handler_;
    bool                                              has_tree_ = false;
};

} // namespace ellindyer::ui::panels
