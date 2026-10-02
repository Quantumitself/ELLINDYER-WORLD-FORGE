#pragma once

#include <cstdint>
#include <string>

#include "Project/ProjectMetadata.hpp"

namespace ellindyer::project::serialization
{

struct ProjectManifest
{
    std::uint32_t format_version = 1;
    std::string   project_id;
    std::string   generator;
    std::string   generator_version;
    ProjectMetadata metadata;

    // Layout hints so that a reader can reconstruct directory structure even
    // if the default layout changes in a future format version.
    std::string schemas_directory;
    std::string entities_directory;
    std::string relationships_directory;
    std::string rules_directory;
    std::string formulas_directory;
    std::string progression_directory;
    std::string narrative_directory;
    std::string quests_directory;
    std::string maps_directory;
    std::string spatial_directory;
    std::string assets_directory;
    std::string localization_directory;
    std::string export_directory;
    std::string tags_directory;
};

} // namespace ellindyer::project::serialization
