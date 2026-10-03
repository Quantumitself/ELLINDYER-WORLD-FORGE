#include "UI/ProjectExplorer/ExplorerNodeState.hpp"

#include <utility>

namespace ellindyer::ui::project_explorer
{

ExplorerNodeState::ExplorerNodeState() = default;

ExplorerNodeState::~ExplorerNodeState() = default;

void ExplorerNodeState::Clear()
{
    expanded_by_key_.clear();
    selected_by_key_.clear();
    selected_id_ = ellindyer::core::Identifier::Nil();
    selected_path_.clear();
}

std::string ExplorerNodeState::KeyFromId(const ellindyer::core::Identifier& id)
{
    return "id:" + id.ToString();
}

std::string ExplorerNodeState::KeyFromPath(const std::filesystem::path& path)
{
    return "path:" + path.lexically_normal().generic_string();
}

bool ExplorerNodeState::IsExpanded(const ellindyer::core::Identifier& id) const noexcept
{
    const auto it = expanded_by_key_.find(KeyFromId(id));
    return it != expanded_by_key_.end() && it->second;
}

bool ExplorerNodeState::IsExpandedByPath(const std::filesystem::path& path) const
{
    if (path.empty())
    {
        return false;
    }
    const auto it = expanded_by_key_.find(KeyFromPath(path));
    return it != expanded_by_key_.end() && it->second;
}

void ExplorerNodeState::SetExpanded(const ellindyer::core::Identifier& id, bool expanded)
{
    if (id.IsNil())
    {
        return;
    }
    expanded_by_key_[KeyFromId(id)] = expanded;
}

void ExplorerNodeState::SetExpandedByPath(const std::filesystem::path& path, bool expanded)
{
    if (path.empty())
    {
        return;
    }
    expanded_by_key_[KeyFromPath(path)] = expanded;
}

bool ExplorerNodeState::IsSelected(const ellindyer::core::Identifier& id) const noexcept
{
    if (id.IsNil())
    {
        return false;
    }
    const auto it = selected_by_key_.find(KeyFromId(id));
    return it != selected_by_key_.end() && it->second;
}

bool ExplorerNodeState::IsSelectedByPath(const std::filesystem::path& path) const
{
    if (path.empty())
    {
        return false;
    }
    const auto it = selected_by_key_.find(KeyFromPath(path));
    return it != selected_by_key_.end() && it->second;
}

void ExplorerNodeState::SetSelected(const ellindyer::core::Identifier& id, bool selected)
{
    selected_by_key_.clear();
    if (selected && !id.IsNil())
    {
        selected_id_ = id;
        selected_by_key_[KeyFromId(id)] = true;
    }
    else
    {
        selected_id_ = ellindyer::core::Identifier::Nil();
    }
}

void ExplorerNodeState::ClearSelection()
{
    selected_by_key_.clear();
    selected_id_ = ellindyer::core::Identifier::Nil();
    selected_path_.clear();
}

void ExplorerNodeState::SetSelectedByPath(const std::filesystem::path& path)
{
    selected_by_key_.clear();
    selected_path_.clear();
    selected_id_ = ellindyer::core::Identifier::Nil();

    if (path.empty())
    {
        return;
    }

    selected_path_ = path;
    selected_by_key_[KeyFromPath(path)] = true;
}

const ellindyer::core::Identifier& ExplorerNodeState::GetSelectedId() const noexcept
{
    return selected_id_;
}

const std::filesystem::path& ExplorerNodeState::GetSelectedPath() const noexcept
{
    return selected_path_;
}

std::size_t ExplorerNodeState::GetExpandedCount() const noexcept
{
    std::size_t count = 0;
    for (const auto& entry : expanded_by_key_)
    {
        if (entry.second)
        {
            ++count;
        }
    }
    return count;
}

} // namespace ellindyer::ui::project_explorer
