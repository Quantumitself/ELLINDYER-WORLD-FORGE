#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Core/Identifier.hpp"

namespace ellindyer::ui::project_explorer
{

class ExplorerNodeState
{
public:
    ExplorerNodeState();
    ~ExplorerNodeState();

    ExplorerNodeState(const ExplorerNodeState&) = delete;
    ExplorerNodeState& operator=(const ExplorerNodeState&) = delete;
    ExplorerNodeState(ExplorerNodeState&&) noexcept = delete;
    ExplorerNodeState& operator=(ExplorerNodeState&&) noexcept = delete;

    void Clear();

    [[nodiscard]] bool IsExpanded(const ellindyer::core::Identifier& id) const noexcept;

    [[nodiscard]] bool IsExpandedByPath(const std::filesystem::path& path) const;

    void SetExpanded(const ellindyer::core::Identifier& id, bool expanded);

    void SetExpandedByPath(const std::filesystem::path& path, bool expanded);

    [[nodiscard]] bool IsSelected(const ellindyer::core::Identifier& id) const noexcept;

    [[nodiscard]] bool IsSelectedByPath(const std::filesystem::path& path) const;

    void SetSelected(const ellindyer::core::Identifier& id, bool selected);

    void ClearSelection();

    [[nodiscard]] const ellindyer::core::Identifier& GetSelectedId() const noexcept;

    [[nodiscard]] const std::filesystem::path& GetSelectedPath() const noexcept;

    void SetSelectedByPath(const std::filesystem::path& path);

    [[nodiscard]] std::size_t GetExpandedCount() const noexcept;

private:
    std::unordered_map<std::string, bool> expanded_by_key_;
    std::unordered_map<std::string, bool> selected_by_key_;
    ellindyer::core::Identifier          selected_id_;
    std::filesystem::path                selected_path_;

    [[nodiscard]] static std::string KeyFromId(const ellindyer::core::Identifier& id);
    [[nodiscard]] static std::string KeyFromPath(const std::filesystem::path& path);
};

} // namespace ellindyer::ui::project_explorer
