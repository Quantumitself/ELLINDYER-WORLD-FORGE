#pragma once

#include <filesystem>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Project/Project.hpp"
#include "Project/Persistence/ProjectPersistencePolicy.hpp"

namespace ellindyer::project::persistence
{

class ProjectRecoveryManager
{
public:
    ProjectRecoveryManager() = delete;

    [[nodiscard]] static bool CanRecover(const std::filesystem::path& project_root);

    [[nodiscard]] static ellindyer::core::Result<void> RecoverFromBackup(
        Project& project,
        const ProjectPersistencePolicy& policy);

    [[nodiscard]] static ellindyer::core::Result<void> DiscardBackups(
        const std::filesystem::path& project_root,
        const ProjectPersistencePolicy& policy);
};

} // namespace ellindyer::project::persistence
