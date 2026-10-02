#pragma once

#include <filesystem>
#include <string>

namespace ellindyer::project
{

struct ProjectLayout
{
    std::filesystem::path root;
    std::filesystem::path manifest_file;
    std::filesystem::path schemas_directory;
    std::filesystem::path entities_directory;
    std::filesystem::path relationships_directory;
    std::filesystem::path rules_directory;
    std::filesystem::path formulas_directory;
    std::filesystem::path progression_directory;
    std::filesystem::path narrative_directory;
    std::filesystem::path quests_directory;
    std::filesystem::path maps_directory;
    std::filesystem::path spatial_directory;
    std::filesystem::path assets_directory;
    std::filesystem::path localization_directory;
    std::filesystem::path export_directory;
    std::filesystem::path tags_directory;

    [[nodiscard]] static ProjectLayout FromRoot(const std::filesystem::path& root);

    [[nodiscard]] bool IsValid() const noexcept;
};

} // namespace ellindyer::project
