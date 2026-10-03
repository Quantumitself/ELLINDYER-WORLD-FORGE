#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "Core/Identifier.hpp"

namespace ellindyer::ui::project_explorer
{

enum class ExplorerNodeKind : std::uint8_t
{
    Root,
    Section,
    Directory,
    File,
    Project
};

struct ExplorerNode
{
    ExplorerNodeKind              kind        = ExplorerNodeKind::Section;
    std::string                   label;
    std::string                   subtitle;
    std::filesystem::path         path;
    ellindyer::core::Identifier   id;
    std::vector<ExplorerNode>     children;
    bool                          expanded = false;
    bool                          selected = false;

    [[nodiscard]] bool HasChildren() const noexcept
    {
        return !children.empty();
    }

    [[nodiscard]] bool IsContainer() const noexcept
    {
        return kind == ExplorerNodeKind::Root
            || kind == ExplorerNodeKind::Section
            || kind == ExplorerNodeKind::Directory
            || kind == ExplorerNodeKind::Project;
    }
};

} // namespace ellindyer::ui::project_explorer
