#include "Project/ProjectLayout.hpp"

namespace ellindyer::project
{

namespace
{

constexpr const char* kManifestFileName         = "Project.wforge";
constexpr const char* kSchemasDirectoryName     = "Schemas";
constexpr const char* kEntitiesDirectoryName    = "Entities";
constexpr const char* kRelationshipsDirectoryName = "Relationships";
constexpr const char* kRulesDirectoryName       = "Rules";
constexpr const char* kFormulasDirectoryName    = "Formulas";
constexpr const char* kProgressionDirectoryName = "Progression";
constexpr const char* kNarrativeDirectoryName   = "Narrative";
constexpr const char* kQuestsDirectoryName      = "Quests";
constexpr const char* kMapsDirectoryName        = "Maps";
constexpr const char* kSpatialDirectoryName     = "Spatial";
constexpr const char* kAssetsDirectoryName      = "Assets";
constexpr const char* kLocalizationDirectoryName = "Localization";
constexpr const char* kExportDirectoryName      = "Export";
constexpr const char* kTagsDirectoryName        = "Tags";

} // namespace

ProjectLayout ProjectLayout::FromRoot(const std::filesystem::path& root)
{
    ProjectLayout layout{};
    layout.root                     = root;
    layout.manifest_file            = root / kManifestFileName;
    layout.schemas_directory        = root / kSchemasDirectoryName;
    layout.entities_directory       = root / kEntitiesDirectoryName;
    layout.relationships_directory  = root / kRelationshipsDirectoryName;
    layout.rules_directory          = root / kRulesDirectoryName;
    layout.formulas_directory       = root / kFormulasDirectoryName;
    layout.progression_directory    = root / kProgressionDirectoryName;
    layout.narrative_directory      = root / kNarrativeDirectoryName;
    layout.quests_directory         = root / kQuestsDirectoryName;
    layout.maps_directory           = root / kMapsDirectoryName;
    layout.spatial_directory        = root / kSpatialDirectoryName;
    layout.assets_directory         = root / kAssetsDirectoryName;
    layout.localization_directory   = root / kLocalizationDirectoryName;
    layout.export_directory         = root / kExportDirectoryName;
    layout.tags_directory           = root / kTagsDirectoryName;
    return layout;
}

bool ProjectLayout::IsValid() const noexcept
{
    return !root.empty() && !manifest_file.empty();
}

} // namespace ellindyer::project
