#include "Project/Persistence/ProjectBackupManager.hpp"

#include <system_error>

#include "Core/ErrorCode.hpp"
#include "Core/FileHelpers.hpp"

namespace ellindyer::project::persistence
{

namespace
{

bool Exists(const std::filesystem::path& path)
{
    std::error_code ec;
    return std::filesystem::exists(path, ec) && !ec;
}

} // namespace

ellindyer::core::Result<void> ProjectBackupManager::RotateBackups(
    const std::filesystem::path& manifest_file,
    const ProjectPersistencePolicy& policy)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (manifest_file.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Manifest file path is empty."));
    }

    if (!Exists(manifest_file))
    {
        return Result<void>{};
    }

    if (policy.max_backups_kept == 0)
    {
        return Result<void>{};
    }

    // Shift existing backups forward, oldest first, so we do not overwrite a
    // backup we still need to move.
    for (std::uint32_t index = policy.max_backups_kept; index > 1; --index)
    {
        const std::filesystem::path older =
            ProjectPersistencePolicy::BackupPathFor(manifest_file, index - 1);
        const std::filesystem::path newer =
            ProjectPersistencePolicy::BackupPathFor(manifest_file, index);

        if (!Exists(older))
        {
            continue;
        }

        std::error_code ec;
        if (Exists(newer))
        {
            std::filesystem::remove(newer, ec);
            if (ec)
            {
                return Result<void>(MakeError(
                    ErrorCode::IoError,
                    "Failed to remove old backup: " + ec.message()));
            }
        }

        std::filesystem::rename(older, newer, ec);
        if (ec)
        {
            return Result<void>(MakeError(
                ErrorCode::IoError,
                "Failed to rotate backup: " + ec.message()));
        }
    }

    const std::filesystem::path slot1 =
        ProjectPersistencePolicy::BackupPathFor(manifest_file, 1);

    std::error_code ec;
    if (Exists(slot1))
    {
        std::filesystem::remove(slot1, ec);
        if (ec)
        {
            return Result<void>(MakeError(
                ErrorCode::IoError,
                "Failed to remove existing slot-1 backup: " + ec.message()));
        }
    }

    std::filesystem::copy_file(
        manifest_file,
        slot1,
        std::filesystem::copy_options::overwrite_existing,
        ec);
    if (ec)
    {
        return Result<void>(MakeError(
            ErrorCode::IoError,
            "Failed to create backup: " + ec.message()));
    }

    return Result<void>{};
}

bool ProjectBackupManager::HasBackup(const std::filesystem::path& manifest_file)
{
    if (manifest_file.empty())
    {
        return false;
    }

    return Exists(ProjectPersistencePolicy::BackupPathFor(manifest_file, 1));
}

std::filesystem::path ProjectBackupManager::LatestBackupPath(
    const std::filesystem::path& manifest_file)
{
    if (manifest_file.empty())
    {
        return {};
    }

    const std::filesystem::path slot1 =
        ProjectPersistencePolicy::BackupPathFor(manifest_file, 1);

    if (Exists(slot1))
    {
        return slot1;
    }

    return {};
}

ellindyer::core::Result<void> ProjectBackupManager::DeleteAllBackups(
    const std::filesystem::path& manifest_file,
    const ProjectPersistencePolicy& policy)
{
    using ellindyer::core::Result;

    if (manifest_file.empty())
    {
        return Result<void>{};
    }

    for (std::uint32_t index = 1; index <= policy.max_backups_kept; ++index)
    {
        const std::filesystem::path backup =
            ProjectPersistencePolicy::BackupPathFor(manifest_file, index);

        std::error_code ec;
        if (Exists(backup))
        {
            std::filesystem::remove(backup, ec);
            if (ec)
            {
                return Result<void>(ellindyer::core::MakeError(
                    ellindyer::core::ErrorCode::IoError,
                    "Failed to remove backup: " + ec.message()));
            }
        }
    }

    return Result<void>{};
}

} // namespace ellindyer::project::persistence
