#include "Project/ProjectPaths.hpp"

#include <sstream>
#include <system_error>

#include "Core/ErrorCode.hpp"
#include "Core/FileHelpers.hpp"

namespace ellindyer::project
{

ellindyer::core::Result<void> ProjectPaths::EnsureSingleDirectory(
    const std::filesystem::path& directory,
    const char* label)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (directory.empty())
    {
        return Result<void>{};
    }

    std::error_code ec;
    if (std::filesystem::exists(directory, ec))
    {
        if (std::filesystem::is_directory(directory, ec))
        {
            return Result<void>{};
        }
        return Result<void>(MakeError(
            ErrorCode::AlreadyExists,
            std::string(label != nullptr ? label : "path") +
                " exists but is not a directory: " + directory.generic_string()));
    }

    std::filesystem::create_directories(directory, ec);
    if (ec)
    {
        return Result<void>(MakeError(
            ErrorCode::IoError,
            std::string("Failed to create directory for ") +
                (label != nullptr ? label : "path") + ": " + ec.message()));
    }

    return Result<void>{};
}

ellindyer::core::Result<void> ProjectPaths::EnsureDirectoriesExist(const ProjectLayout& layout)
{
    using ellindyer::core::Result;

    Result<void> root_ready = EnsureSingleDirectory(layout.root, "project root");
    if (root_ready.HasError())
    {
        return root_ready;
    }

    const std::pair<std::filesystem::path, const char*> directories[] = {
        {layout.schemas_directory,       "schemas"},
        {layout.entities_directory,      "entities"},
        {layout.relationships_directory, "relationships"},
        {layout.rules_directory,         "rules"},
        {layout.formulas_directory,      "formulas"},
        {layout.progression_directory,   "progression"},
        {layout.narrative_directory,     "narrative"},
        {layout.quests_directory,        "quests"},
        {layout.maps_directory,          "maps"},
        {layout.spatial_directory,       "spatial"},
        {layout.assets_directory,        "assets"},
        {layout.localization_directory,  "localization"},
        {layout.export_directory,        "export"},
        {layout.tags_directory,          "tags"},
    };

    for (const auto& entry : directories)
    {
        Result<void> ready = EnsureSingleDirectory(entry.first, entry.second);
        if (ready.HasError())
        {
            return ready;
        }
    }

    return Result<void>{};
}

std::string ProjectPaths::Describe(const ProjectLayout& layout)
{
    std::ostringstream stream;
    stream << "root: "                  << layout.root.generic_string()                  << '\n';
    stream << "manifest: "              << layout.manifest_file.generic_string()         << '\n';
    stream << "schemas: "               << layout.schemas_directory.generic_string()    << '\n';
    stream << "entities: "              << layout.entities_directory.generic_string()   << '\n';
    stream << "relationships: "         << layout.relationships_directory.generic_string() << '\n';
    stream << "rules: "                 << layout.rules_directory.generic_string()      << '\n';
    stream << "formulas: "              << layout.formulas_directory.generic_string()   << '\n';
    stream << "progression: "           << layout.progression_directory.generic_string() << '\n';
    stream << "narrative: "             << layout.narrative_directory.generic_string()  << '\n';
    stream << "quests: "                << layout.quests_directory.generic_string()     << '\n';
    stream << "maps: "                  << layout.maps_directory.generic_string()       << '\n';
    stream << "spatial: "               << layout.spatial_directory.generic_string()    << '\n';
    stream << "assets: "                << layout.assets_directory.generic_string()     << '\n';
    stream << "localization: "          << layout.localization_directory.generic_string() << '\n';
    stream << "export: "                << layout.export_directory.generic_string()     << '\n';
    stream << "tags: "                  << layout.tags_directory.generic_string();
    return stream.str();
}

} // namespace ellindyer::project
