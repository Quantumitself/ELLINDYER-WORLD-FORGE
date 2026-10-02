#pragma once

#include <cstdint>
#include <filesystem>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Project/Persistence/ProjectPersistencePolicy.hpp"

namespace ellindyer::project::persistence
{

class ProjectBackupManager
{
public:
    ProjectBackupManager() = delete;

    [[nodiscard]] static ellindyer::core::Result<void> RotateBackups(
        const std::filesystem::path& manifest_file,
        const ProjectPersistencePolicy& policy);

    [[nodiscard]] static bool HasBackup(const std::filesystem::path& manifest_file);

    [[nodiscard]] static std::filesystem::path LatestBackupPath(
        const std::filesystem::path& manifest_file);

    [[nodiscard]] static ellindyer::core::Result<void> DeleteAllBackups(
        const std::filesystem::path& manifest_file,
        const ProjectPersistencePolicy& policy);
};

} // namespace ellindyer::project::persistence
