#pragma once

#include <cstdint>
#include <filesystem>

namespace ellindyer::project::persistence
{

struct ProjectPersistencePolicy
{
    std::uint32_t max_manifest_bytes     = 4U * 1024U * 1024U;
    std::uint32_t max_metadata_name_len  = 128;
    std::uint32_t max_metadata_desc_len  = 4096;
    std::uint32_t max_metadata_author_len = 128;
    std::uint32_t max_metadata_world_len = 128;
    std::uint32_t max_metadata_world_desc_len = 4096;
    std::uint32_t max_metadata_version_len = 64;
    std::uint32_t max_metadata_engine_len = 64;
    std::uint32_t max_metadata_org_len   = 128;
    bool          create_backup_on_save  = true;
    std::uint32_t max_backups_kept       = 3;

    [[nodiscard]] static const ProjectPersistencePolicy& Default() noexcept;

    [[nodiscard]] static std::filesystem::path BackupPathFor(
        const std::filesystem::path& manifest_file,
        std::uint32_t slot_index);
};

} // namespace ellindyer::project::persistence
