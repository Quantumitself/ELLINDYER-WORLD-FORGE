#pragma once

#include <filesystem>
#include <string>

#include "Project/Project.hpp"
#include "UI/ProjectExplorer/ExplorerNode.hpp"

namespace ellindyer::ui::project_explorer
{

struct ExplorerBuildOptions
{
    bool show_hidden      = false;
    bool follow_symlinks  = false;
    std::uint32_t max_children_per_directory = 512;
};

class ExplorerNodeBuilder
{
public:
    ExplorerNodeBuilder() = delete;

    [[nodiscard]] static ExplorerNode BuildFromProject(
        const ellindyer::project::Project& project,
        const ExplorerBuildOptions& options);

    [[nodiscard]] static ExplorerNode BuildEmpty();
};

} // namespace ellindyer::ui::project_explorer
