#include "UI/ProjectExplorer/ProjectExplorerTree.hpp"

#include <utility>

namespace ellindyer::ui::project_explorer
{

namespace
{

const char* GlyphForNode(const ExplorerNode& node) noexcept
{
    switch (node.kind)
    {
    case ExplorerNodeKind::Root:      return "[*]";
    case ExplorerNodeKind::Project:   return "[P]";
    case ExplorerNodeKind::Section:   return "[+]";
    case ExplorerNodeKind::Directory: return "[d]";
    case ExplorerNodeKind::File:      return "[-]";
    }
    return "[ ]";
}

} // namespace

ProjectExplorerTree::ProjectExplorerTree() = default;

ProjectExplorerTree::~ProjectExplorerTree() = default;

void ProjectExplorerTree::SetStyle(const ProjectExplorerTreeStyle& style)
{
    style_ = style;
}

const ProjectExplorerTreeStyle& ProjectExplorerTree::GetStyle() const noexcept
{
    return style_;
}

void ProjectExplorerTree::SetRoot(const ExplorerNode& root)
{
    root_ = root;
    has_root_ = true;

    if (!state_.IsExpanded(root_.id))
    {
        state_.SetExpanded(root_.id, root_.expanded);
    }
}

void ProjectExplorerTree::Clear()
{
    root_ = ExplorerNode{};
    has_root_ = false;
    state_.Clear();
}

bool ProjectExplorerTree::HasRoot() const noexcept
{
    return has_root_;
}

const ExplorerNode& ProjectExplorerTree::GetRoot() const noexcept
{
    return root_;
}

ExplorerNodeState& ProjectExplorerTree::GetState() noexcept
{
    return state_;
}

const ExplorerNodeState& ProjectExplorerTree::GetState() const noexcept
{
    return state_;
}

void ProjectExplorerTree::SetSelectionHandler(SelectionHandler handler)
{
    selection_handler_ = std::move(handler);
}

void ProjectExplorerTree::SetActivationHandler(ActivationHandler handler)
{
    activation_handler_ = std::move(handler);
}

void ProjectExplorerTree::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (!has_root_)
    {
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 2.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 2.0f));

    RenderNode(root_, fonts, 0);

    ImGui::PopStyleVar(2);
}

void ProjectExplorerTree::ExpandPath(const std::filesystem::path& path)
{
    if (path.empty())
    {
        return;
    }
    state_.SetExpandedByPath(path, true);
}

void ProjectExplorerTree::SelectPath(const std::filesystem::path& path)
{
    state_.SetSelectedByPath(path);
}

void ProjectExplorerTree::RenderNode(const ExplorerNode& node,
                                     const ellindyer::ui::fonts::FontSet& fonts,
                                     int depth)
{
    const bool is_container = node.IsContainer();
    const bool has_children = node.HasChildren();

    const bool expanded = is_container
        && (state_.IsExpanded(node.id) || state_.IsExpandedByPath(node.path));

    const bool selected = state_.IsSelected(node.id)
                       || state_.IsSelectedByPath(node.path);

    ImGui::PushID(node.id.ToString().c_str());

    const float start_x = static_cast<float>(depth) * style_.indent_width;
    if (start_x > 0.0f)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + start_x);
    }

    const ImVec2 row_start = ImGui::GetCursorScreenPos();
    const float available_width = ImGui::GetContentRegionAvail().x;
    const float row_width = available_width - start_x;

    const ImVec2 row_end(row_start.x + row_width, row_start.y + style_.row_height);

    const bool hovered = ImGui::IsMouseHoveringRect(row_start, row_end);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    if (selected || hovered)
    {
        draw_list->AddRectFilled(
            row_start, row_end,
            ImGui::GetColorU32(selected ? style_.selection_color : style_.hover_color));
    }

    if (is_container && has_children)
    {
        const char* arrow = expanded ? "v" : ">";
        draw_list->AddText(ImVec2(row_start.x + 2.0f, row_start.y + 2.0f),
                           ImGui::GetColorU32(style_.branch_color),
                           arrow);
    }

    ImGui::SetCursorScreenPos(ImVec2(row_start.x + style_.icon_column_width,
                                     row_start.y + 1.0f));

    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, style_.primary_text);
    ImGui::TextUnformatted(node.label.c_str());
    ImGui::PopStyleColor();

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }

    if (!node.subtitle.empty())
    {
        ImGui::SameLine();

        if (fonts.small_regular != nullptr)
        {
            ImGui::PushFont(fonts.small_regular, fonts.small_regular->LegacySize);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, style_.secondary_text);
        ImGui::TextUnformatted(node.subtitle.c_str());
        ImGui::PopStyleColor();

        if (fonts.small_regular != nullptr)
        {
            ImGui::PopFont();
        }
    }

    ImGui::SetCursorScreenPos(row_start);
    ImGui::InvisibleButton("##ExplorerRow",
                           ImVec2(row_width, style_.row_height));

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        if (is_container && has_children)
        {
            const bool new_expanded = !expanded;
            state_.SetExpanded(node.id, new_expanded);
            if (!node.path.empty())
            {
                state_.SetExpandedByPath(node.path, new_expanded);
            }
        }

        state_.SetSelected(node.id, true);
        if (!node.path.empty())
        {
            state_.SetSelectedByPath(node.path);
        }

        if (selection_handler_)
        {
            selection_handler_(node);
        }
    }

    if (ImGui::IsItemHovered()
        && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        if (activation_handler_)
        {
            activation_handler_(node);
        }
    }

    ImGui::PopID();

    if (is_container && expanded && has_children)
    {
        for (const ExplorerNode& child : node.children)
        {
            RenderNode(child, fonts, depth + 1);
        }
    }
}

} // namespace ellindyer::ui::project_explorer
