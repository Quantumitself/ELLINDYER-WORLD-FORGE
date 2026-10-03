#include "UI/ProjectExplorer/ExplorerNodeBuilder.hpp"

#include <algorithm>
#include <system_error>
#include <utility>

#include "Core/IdentifierGenerator.hpp"

namespace ellindyer::ui::project_explorer
{

namespace
{

ExplorerNode MakeSectionNode(const std::string& label,
                             const std::filesystem::path& path)
{
    ExplorerNode node{};
    node.kind   = ExplorerNodeKind::Section;
    node.label  = label;
    node.path   = path;
    node.id     = ellindyer::core::IdentifierGenerator::NewIdentifier();
    return node;
}

ExplorerNode MakeFileNode(const std::filesystem::path& path)
{
    ExplorerNode node{};
    node.kind  = ExplorerNodeKind::File;
    node.label = path.filename().string();
    node.path  = path;
    node.id    = ellindyer::core::IdentifierGenerator::NewIdentifier();
    return node;
}

bool IsHiddenEntry(const std::filesystem::directory_entry& entry)
{
    const std::string name = entry.path().filename().string();
    return !name.empty() && name.front() == '.';
}

bool CompareDirectoryEntries(const std::filesystem::directory_entry& lhs,
                             const std::filesystem::directory_entry& rhs)
{
    std::error_code lhs_ec;
    std::error_code rhs_ec;
    const bool lhs_dir = lhs.is_directory(lhs_ec) && !lhs_ec;
    const bool rhs_dir = rhs.is_directory(rhs_ec) && !rhs_ec;

    if (lhs_dir != rhs_dir)
    {
        return lhs_dir && !rhs_dir;
    }

    return lhs.path().filename().string()
         < rhs.path().filename().string();
}

void EnumerateDirectory(const std::filesystem::path& directory,
                        ExplorerNode& out_node,
                        const ExplorerBuildOptions& options)
{
    std::error_code ec;
    if (!std::filesystem::exists(directory, ec) || ec)
    {
        return;
    }
    if (!std::filesystem::is_directory(directory, ec) || ec)
    {
        return;
    }

    std::vector<std::filesystem::directory_entry> entries;
    std::filesystem::directory_iterator it(directory, ec);
    if (ec)
    {
        return;
    }

    for (const std::filesystem::directory_entry& entry : it)
    {
        if (!options.show_hidden && IsHiddenEntry(entry))
        {
            continue;
        }

        if (!options.follow_symlinks)
        {
            std::error_code symlink_ec;
            const auto status = entry.symlink_status(symlink_ec);
            if (!symlink_ec && std::filesystem::is_symlink(status))
            {
                continue;
            }
        }

        entries.push_back(entry);
        if (entries.size() >= options.max_children_per_directory)
        {
            break;
        }
    }

    std::sort(entries.begin(), entries.end(), CompareDirectoryEntries);

    for (const std::filesystem::directory_entry& entry : entries)
    {
        std::error_code type_ec;
        const bool entry_is_directory = entry.is_directory(type_ec) && !type_ec;

        if (entry_is_directory)
        {
            ExplorerNode child{};
            child.kind  = ExplorerNodeKind::Directory;
            child.label = entry.path().filename().string();
            child.path  = entry.path();
            child.id    = ellindyer::core::IdentifierGenerator::NewIdentifier();
            out_node.children.push_back(std::move(child));
        }
        else if (entry.is_regular_file(type_ec) && !type_ec)
        {
            out_node.children.push_back(MakeFileNode(entry.path()));
        }
    }
}

void BuildSectionsForProject(const ellindyer::project::Project& project,
                             ExplorerNode& root,
                             const ExplorerBuildOptions& options)
{
    const ellindyer::project::ProjectLayout& layout = project.GetLayout();

    const struct SectionDefinition
    {
        const char*              label;
        std::filesystem::path    path;
    } sections[] = {
        { "World",         layout.root },
        { "Schemas",       layout.schemas_directory },
        { "Entities",      layout.entities_directory },
        { "Relationships", layout.relationships_directory },
        { "Rules",         layout.rules_directory },
        { "Formulas",      layout.formulas_directory },
        { "Progression",   layout.progression_directory },
        { "Narrative",     layout.narrative_directory },
        { "Quests",        layout.quests_directory },
        { "Maps",          layout.maps_directory },
        { "Spatial",       layout.spatial_directory },
        { "Assets",        layout.assets_directory },
        { "Localization",  layout.localization_directory },
        { "Export",        layout.export_directory },
        { "Tags",          layout.tags_directory },
    };

    for (const SectionDefinition& section : sections)
    {
        ExplorerNode section_node = MakeSectionNode(section.label, section.path);
        EnumerateDirectory(section.path, section_node, options);
        root.children.push_back(std::move(section_node));
    }
}

} // namespace

ExplorerNode ExplorerNodeBuilder::BuildFromProject(
    const ellindyer::project::Project& project,
    const ExplorerBuildOptions& options)
{
    ExplorerNode root{};
    root.kind     = ExplorerNodeKind::Root;
    root.label    = project.GetDisplayName();
    root.path     = project.GetLayout().root;
    root.id       = project.GetId();
    root.expanded = true;

    if (!project.IsOpen()
        && project.GetState() != ellindyer::project::ProjectState::Initialized)
    {
        return root;
    }

    BuildSectionsForProject(project, root, options);
    return root;
}

ExplorerNode ExplorerNodeBuilder::BuildEmpty()
{
    ExplorerNode root{};
    root.kind     = ExplorerNodeKind::Root;
    root.label    = "No project loaded";
    root.expanded = true;
    return root;
}

} // namespace ellindyer::ui::project_explorer
