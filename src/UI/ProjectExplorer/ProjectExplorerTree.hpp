#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>

#include <imgui.h>

#include "UI/Fonts/FontManager.hpp"
#include "UI/ProjectExplorer/ExplorerNode.hpp"
#include "UI/ProjectExplorer/ExplorerNodeState.hpp"

namespace ellindyer::ui::project_explorer
{

struct ProjectExplorerTreeStyle
{
    float   indent_width      = 14.0f;
    float   row_height        = 20.0f;
    float   icon_column_width = 16.0f;
    ImVec4  primary_text      = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    ImVec4  secondary_text    = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
    ImVec4  muted_text        = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    ImVec4  selection_color   = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    ImVec4  hover_color       = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    ImVec4  branch_color      = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
};

class ProjectExplorerTree
{
public:
    using SelectionHandler = std::function<void(const ExplorerNode&)>;
    using ActivationHandler = std::function<void(const ExplorerNode&)>;

    ProjectExplorerTree();
    ~ProjectExplorerTree();

    ProjectExplorerTree(const ProjectExplorerTree&) = delete;
    ProjectExplorerTree& operator=(const ProjectExplorerTree&) = delete;
    ProjectExplorerTree(ProjectExplorerTree&&) noexcept = delete;
    ProjectExplorerTree& operator=(ProjectExplorerTree&&) noexcept = delete;

    void SetStyle(const ProjectExplorerTreeStyle& style);

    [[nodiscard]] const ProjectExplorerTreeStyle& GetStyle() const noexcept;

    void SetRoot(const ExplorerNode& root);

    void Clear();

    [[nodiscard]] bool HasRoot() const noexcept;

    [[nodiscard]] const ExplorerNode& GetRoot() const noexcept;

    [[nodiscard]] ExplorerNodeState& GetState() noexcept;

    [[nodiscard]] const ExplorerNodeState& GetState() const noexcept;

    void SetSelectionHandler(SelectionHandler handler);

    void SetActivationHandler(ActivationHandler handler);

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

    void ExpandPath(const std::filesystem::path& path);

    void SelectPath(const std::filesystem::path& path);

private:
    void RenderNode(const ExplorerNode& node,
                    const ellindyer::ui::fonts::FontSet& fonts,
                    int depth);

    ProjectExplorerTreeStyle  style_{};
    ExplorerNode              root_{};
    bool                      has_root_ = false;
    ExplorerNodeState         state_{};
    SelectionHandler          selection_handler_;
    ActivationHandler         activation_handler_;
};

} // namespace ellindyer::ui::project_explorer
