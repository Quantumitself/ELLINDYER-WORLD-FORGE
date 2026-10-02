#include "Project/Serialization/ProjectManifestCodec.hpp"

#include <string>

#include <nlohmann/json.hpp>

#include "Core/ErrorCode.hpp"
#include "Project/ProjectIdentifier.hpp"
#include "Project/Serialization/ProjectManifestSchema.hpp"

namespace ellindyer::project::serialization
{

namespace
{

using nlohmann::json;

constexpr const char* kGeneratorName    = "EllindyerWorldForge";
constexpr const char* kGeneratorVersion = "0.1.0";

std::string ReadString(const json& object, std::string_view key, std::string fallback = std::string{})
{
    const auto it = object.find(std::string(key));
    if (it == object.end() || !it->is_string())
    {
        return fallback;
    }
    return it->get<std::string>();
}

std::uint32_t ReadUInt32(const json& object, std::string_view key, std::uint32_t fallback)
{
    const auto it = object.find(std::string(key));
    if (it == object.end() || !it->is_number_unsigned())
    {
        return fallback;
    }
    return it->get<std::uint32_t>();
}

} // namespace

ProjectManifest ProjectManifestCodec::FromProject(const Project& project)
{
    ProjectManifest manifest{};
    manifest.format_version    = ProjectManifestSchema::CurrentFormatVersion();
    manifest.project_id        = project.GetId().ToString();
    manifest.generator         = kGeneratorName;
    manifest.generator_version = kGeneratorVersion;
    manifest.metadata          = project.GetMetadata();

    const ProjectLayout& layout = project.GetLayout();
    manifest.schemas_directory       = layout.schemas_directory.filename().string();
    manifest.entities_directory      = layout.entities_directory.filename().string();
    manifest.relationships_directory = layout.relationships_directory.filename().string();
    manifest.rules_directory         = layout.rules_directory.filename().string();
    manifest.formulas_directory      = layout.formulas_directory.filename().string();
    manifest.progression_directory   = layout.progression_directory.filename().string();
    manifest.narrative_directory     = layout.narrative_directory.filename().string();
    manifest.quests_directory        = layout.quests_directory.filename().string();
    manifest.maps_directory          = layout.maps_directory.filename().string();
    manifest.spatial_directory       = layout.spatial_directory.filename().string();
    manifest.assets_directory        = layout.assets_directory.filename().string();
    manifest.localization_directory  = layout.localization_directory.filename().string();
    manifest.export_directory        = layout.export_directory.filename().string();
    manifest.tags_directory          = layout.tags_directory.filename().string();

    return manifest;
}

ellindyer::core::Result<void> ProjectManifestCodec::ApplyToProject(
    const ProjectManifest& manifest,
    Project& project)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (manifest.format_version == 0)
    {
        return Result<void>(MakeError(ErrorCode::InvalidProject,
                                      "Manifest formatVersion is zero."));
    }

    if (manifest.format_version > ProjectManifestSchema::CurrentFormatVersion())
    {
        return Result<void>(MakeError(ErrorCode::InvalidProject,
                                      "Manifest formatVersion is newer than supported."));
    }

    ellindyer::core::Identifier parsed_id{};
    if (!manifest.project_id.empty())
    {
        if (!ellindyer::core::Identifier::TryParse(manifest.project_id, parsed_id))
        {
            return Result<void>(MakeError(ErrorCode::InvalidProject,
                                          "Manifest projectId is malformed."));
        }
    }
    else
    {
        parsed_id = MakeProjectId();
    }

    project.SetId(parsed_id);
    project.SetMetadata(manifest.metadata);

    ProjectLayout layout = project.GetLayout();
    if (!manifest.schemas_directory.empty())
    {
        layout.schemas_directory = layout.root / manifest.schemas_directory;
    }
    if (!manifest.entities_directory.empty())
    {
        layout.entities_directory = layout.root / manifest.entities_directory;
    }
    if (!manifest.relationships_directory.empty())
    {
        layout.relationships_directory = layout.root / manifest.relationships_directory;
    }
    if (!manifest.rules_directory.empty())
    {
        layout.rules_directory = layout.root / manifest.rules_directory;
    }
    if (!manifest.formulas_directory.empty())
    {
        layout.formulas_directory = layout.root / manifest.formulas_directory;
    }
    if (!manifest.progression_directory.empty())
    {
        layout.progression_directory = layout.root / manifest.progression_directory;
    }
    if (!manifest.narrative_directory.empty())
    {
        layout.narrative_directory = layout.root / manifest.narrative_directory;
    }
    if (!manifest.quests_directory.empty())
    {
        layout.quests_directory = layout.root / manifest.quests_directory;
    }
    if (!manifest.maps_directory.empty())
    {
        layout.maps_directory = layout.root / manifest.maps_directory;
    }
    if (!manifest.spatial_directory.empty())
    {
        layout.spatial_directory = layout.root / manifest.spatial_directory;
    }
    if (!manifest.assets_directory.empty())
    {
        layout.assets_directory = layout.root / manifest.assets_directory;
    }
    if (!manifest.localization_directory.empty())
    {
        layout.localization_directory = layout.root / manifest.localization_directory;
    }
    if (!manifest.export_directory.empty())
    {
        layout.export_directory = layout.root / manifest.export_directory;
    }
    if (!manifest.tags_directory.empty())
    {
        layout.tags_directory = layout.root / manifest.tags_directory;
    }
    project.SetLayout(layout);

    project.SetState(ProjectState::Loaded);
    project.ClearModified();
    return Result<void>{};
}

ellindyer::core::Result<std::string> ProjectManifestCodec::EncodeToString(
    const ProjectManifest& manifest)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    try
    {
        json root;
        root[std::string(ProjectManifestSchema::FormatVersionKey())]    = manifest.format_version;
        root[std::string(ProjectManifestSchema::ProjectIdKey())]        = manifest.project_id;
        root[std::string(ProjectManifestSchema::GeneratorKey())]        = manifest.generator;
        root[std::string(ProjectManifestSchema::GeneratorVersionKey())] = manifest.generator_version;

        json metadata;
        metadata[std::string(ProjectManifestSchema::MetadataNameKey())]
            = manifest.metadata.name;
        metadata[std::string(ProjectManifestSchema::MetadataDescriptionKey())]
            = manifest.metadata.description;
        metadata[std::string(ProjectManifestSchema::MetadataAuthorKey())]
            = manifest.metadata.author;
        metadata[std::string(ProjectManifestSchema::MetadataOrganizationKey())]
            = manifest.metadata.organization;
        metadata[std::string(ProjectManifestSchema::MetadataWorldNameKey())]
            = manifest.metadata.world_name;
        metadata[std::string(ProjectManifestSchema::MetadataWorldDescriptionKey())]
            = manifest.metadata.world_description;
        metadata[std::string(ProjectManifestSchema::MetadataCreatedUtcKey())]
            = manifest.metadata.created_utc;
        metadata[std::string(ProjectManifestSchema::MetadataModifiedUtcKey())]
            = manifest.metadata.modified_utc;
        metadata[std::string(ProjectManifestSchema::MetadataEngineTargetKey())]
            = manifest.metadata.engine_target;
        metadata[std::string(ProjectManifestSchema::MetadataProjectVersionKey())]
            = manifest.metadata.project_version;

        root[std::string(ProjectManifestSchema::MetadataKey())] = metadata;

        json layout;
        layout[std::string(ProjectManifestSchema::LayoutSchemasKey())]
            = manifest.schemas_directory;
        layout[std::string(ProjectManifestSchema::LayoutEntitiesKey())]
            = manifest.entities_directory;
        layout[std::string(ProjectManifestSchema::LayoutRelationshipsKey())]
            = manifest.relationships_directory;
        layout[std::string(ProjectManifestSchema::LayoutRulesKey())]
            = manifest.rules_directory;
        layout[std::string(ProjectManifestSchema::LayoutFormulasKey())]
            = manifest.formulas_directory;
        layout[std::string(ProjectManifestSchema::LayoutProgressionKey())]
            = manifest.progression_directory;
        layout[std::string(ProjectManifestSchema::LayoutNarrativeKey())]
            = manifest.narrative_directory;
        layout[std::string(ProjectManifestSchema::LayoutQuestsKey())]
            = manifest.quests_directory;
        layout[std::string(ProjectManifestSchema::LayoutMapsKey())]
            = manifest.maps_directory;
        layout[std::string(ProjectManifestSchema::LayoutSpatialKey())]
            = manifest.spatial_directory;
        layout[std::string(ProjectManifestSchema::LayoutAssetsKey())]
            = manifest.assets_directory;
        layout[std::string(ProjectManifestSchema::LayoutLocalizationKey())]
            = manifest.localization_directory;
        layout[std::string(ProjectManifestSchema::LayoutExportKey())]
            = manifest.export_directory;
        layout[std::string(ProjectManifestSchema::LayoutTagsKey())]
            = manifest.tags_directory;

        root[std::string(ProjectManifestSchema::LayoutKey())] = layout;

        return Result<std::string>(root.dump(4));
    }
    catch (const std::exception& exception)
    {
        return Result<std::string>(MakeError(
            ErrorCode::SerializationError,
            std::string("Manifest encoding failed: ") + exception.what()));
    }
}

ellindyer::core::Result<ProjectManifest> ProjectManifestCodec::DecodeFromString(
    std::string_view text)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (text.empty())
    {
        return Result<ProjectManifest>(MakeError(ErrorCode::InvalidProject,
                                                 "Manifest text is empty."));
    }

    json root;
    try
    {
        root = json::parse(text.begin(), text.end());
    }
    catch (const std::exception& exception)
    {
        return Result<ProjectManifest>(MakeError(
            ErrorCode::ParseError,
            std::string("Manifest JSON parse failed: ") + exception.what()));
    }

    if (!root.is_object())
    {
        return Result<ProjectManifest>(MakeError(
            ErrorCode::InvalidProject,
            "Manifest root is not a JSON object."));
    }

    ProjectManifest manifest{};
    manifest.format_version = ReadUInt32(
        root, ProjectManifestSchema::FormatVersionKey(),
        ProjectManifestSchema::CurrentFormatVersion());

    if (manifest.format_version == 0)
    {
        return Result<ProjectManifest>(MakeError(
            ErrorCode::InvalidProject,
            "Manifest formatVersion is zero."));
    }

    if (manifest.format_version > ProjectManifestSchema::CurrentFormatVersion())
    {
        return Result<ProjectManifest>(MakeError(
            ErrorCode::InvalidProject,
            "Manifest formatVersion is newer than supported."));
    }

    manifest.project_id        = ReadString(root, ProjectManifestSchema::ProjectIdKey());
    manifest.generator         = ReadString(root, ProjectManifestSchema::GeneratorKey());
    manifest.generator_version = ReadString(root, ProjectManifestSchema::GeneratorVersionKey());

    const auto metadata_it = root.find(std::string(ProjectManifestSchema::MetadataKey()));
    if (metadata_it != root.end() && metadata_it->is_object())
    {
        const json& metadata = *metadata_it;
        manifest.metadata.name              = ReadString(metadata, ProjectManifestSchema::MetadataNameKey());
        manifest.metadata.description       = ReadString(metadata, ProjectManifestSchema::MetadataDescriptionKey());
        manifest.metadata.author            = ReadString(metadata, ProjectManifestSchema::MetadataAuthorKey());
        manifest.metadata.organization      = ReadString(metadata, ProjectManifestSchema::MetadataOrganizationKey());
        manifest.metadata.world_name        = ReadString(metadata, ProjectManifestSchema::MetadataWorldNameKey());
        manifest.metadata.world_description = ReadString(metadata, ProjectManifestSchema::MetadataWorldDescriptionKey());
        manifest.metadata.created_utc       = ReadString(metadata, ProjectManifestSchema::MetadataCreatedUtcKey());
        manifest.metadata.modified_utc      = ReadString(metadata, ProjectManifestSchema::MetadataModifiedUtcKey());
        manifest.metadata.engine_target     = ReadString(metadata, ProjectManifestSchema::MetadataEngineTargetKey());
        manifest.metadata.project_version   = ReadString(metadata, ProjectManifestSchema::MetadataProjectVersionKey());
    }

    const auto layout_it = root.find(std::string(ProjectManifestSchema::LayoutKey()));
    if (layout_it != root.end() && layout_it->is_object())
    {
        const json& layout = *layout_it;
        manifest.schemas_directory       = ReadString(layout, ProjectManifestSchema::LayoutSchemasKey());
        manifest.entities_directory      = ReadString(layout, ProjectManifestSchema::LayoutEntitiesKey());
        manifest.relationships_directory = ReadString(layout, ProjectManifestSchema::LayoutRelationshipsKey());
        manifest.rules_directory         = ReadString(layout, ProjectManifestSchema::LayoutRulesKey());
        manifest.formulas_directory      = ReadString(layout, ProjectManifestSchema::LayoutFormulasKey());
        manifest.progression_directory   = ReadString(layout, ProjectManifestSchema::LayoutProgressionKey());
        manifest.narrative_directory     = ReadString(layout, ProjectManifestSchema::LayoutNarrativeKey());
        manifest.quests_directory        = ReadString(layout, ProjectManifestSchema::LayoutQuestsKey());
        manifest.maps_directory          = ReadString(layout, ProjectManifestSchema::LayoutMapsKey());
        manifest.spatial_directory       = ReadString(layout, ProjectManifestSchema::LayoutSpatialKey());
        manifest.assets_directory        = ReadString(layout, ProjectManifestSchema::LayoutAssetsKey());
        manifest.localization_directory  = ReadString(layout, ProjectManifestSchema::LayoutLocalizationKey());
        manifest.export_directory        = ReadString(layout, ProjectManifestSchema::LayoutExportKey());
        manifest.tags_directory          = ReadString(layout, ProjectManifestSchema::LayoutTagsKey());
    }

    if (manifest.metadata.format_version == 0)
    {
        manifest.metadata.format_version = manifest.format_version;
    }
    else
    {
        manifest.metadata.format_version = manifest.format_version;
    }

    return Result<ProjectManifest>(manifest);
}

} // namespace ellindyer::project::serialization
