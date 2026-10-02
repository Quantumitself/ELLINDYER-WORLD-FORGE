#pragma once

#include <string_view>

namespace ellindyer::project::serialization
{

class ProjectManifestSchema
{
public:
    ProjectManifestSchema() = delete;

    [[nodiscard]] static constexpr std::string_view FormatVersionKey() noexcept
    {
        return "formatVersion";
    }

    [[nodiscard]] static constexpr std::string_view ProjectIdKey() noexcept
    {
        return "projectId";
    }

    [[nodiscard]] static constexpr std::string_view GeneratorKey() noexcept
    {
        return "generator";
    }

    [[nodiscard]] static constexpr std::string_view GeneratorVersionKey() noexcept
    {
        return "generatorVersion";
    }

    [[nodiscard]] static constexpr std::string_view MetadataKey() noexcept
    {
        return "metadata";
    }

    [[nodiscard]] static constexpr std::string_view MetadataNameKey() noexcept
    {
        return "name";
    }

    [[nodiscard]] static constexpr std::string_view MetadataDescriptionKey() noexcept
    {
        return "description";
    }

    [[nodiscard]] static constexpr std::string_view MetadataAuthorKey() noexcept
    {
        return "author";
    }

    [[nodiscard]] static constexpr std::string_view MetadataOrganizationKey() noexcept
    {
        return "organization";
    }

    [[nodiscard]] static constexpr std::string_view MetadataWorldNameKey() noexcept
    {
        return "worldName";
    }

    [[nodiscard]] static constexpr std::string_view MetadataWorldDescriptionKey() noexcept
    {
        return "worldDescription";
    }

    [[nodiscard]] static constexpr std::string_view MetadataCreatedUtcKey() noexcept
    {
        return "createdUtc";
    }

    [[nodiscard]] static constexpr std::string_view MetadataModifiedUtcKey() noexcept
    {
        return "modifiedUtc";
    }

    [[nodiscard]] static constexpr std::string_view MetadataEngineTargetKey() noexcept
    {
        return "engineTarget";
    }

    [[nodiscard]] static constexpr std::string_view MetadataProjectVersionKey() noexcept
    {
        return "projectVersion";
    }

    [[nodiscard]] static constexpr std::string_view LayoutKey() noexcept
    {
        return "layout";
    }

    [[nodiscard]] static constexpr std::string_view LayoutSchemasKey() noexcept
    {
        return "schemas";
    }

    [[nodiscard]] static constexpr std::string_view LayoutEntitiesKey() noexcept
    {
        return "entities";
    }

    [[nodiscard]] static constexpr std::string_view LayoutRelationshipsKey() noexcept
    {
        return "relationships";
    }

    [[nodiscard]] static constexpr std::string_view LayoutRulesKey() noexcept
    {
        return "rules";
    }

    [[nodiscard]] static constexpr std::string_view LayoutFormulasKey() noexcept
    {
        return "formulas";
    }

    [[nodiscard]] static constexpr std::string_view LayoutProgressionKey() noexcept
    {
        return "progression";
    }

    [[nodiscard]] static constexpr std::string_view LayoutNarrativeKey() noexcept
    {
        return "narrative";
    }

    [[nodiscard]] static constexpr std::string_view LayoutQuestsKey() noexcept
    {
        return "quests";
    }

    [[nodiscard]] static constexpr std::string_view LayoutMapsKey() noexcept
    {
        return "maps";
    }

    [[nodiscard]] static constexpr std::string_view LayoutSpatialKey() noexcept
    {
        return "spatial";
    }

    [[nodiscard]] static constexpr std::string_view LayoutAssetsKey() noexcept
    {
        return "assets";
    }

    [[nodiscard]] static constexpr std::string_view LayoutLocalizationKey() noexcept
    {
        return "localization";
    }

    [[nodiscard]] static constexpr std::string_view LayoutExportKey() noexcept
    {
        return "export";
    }

    [[nodiscard]] static constexpr std::string_view LayoutTagsKey() noexcept
    {
        return "tags";
    }

    [[nodiscard]] static constexpr std::uint32_t CurrentFormatVersion() noexcept
    {
        return 1U;
    }
};

} // namespace ellindyer::project::serialization
