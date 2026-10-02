#pragma once

#include <filesystem>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Project/Project.hpp"
#include "Project/Persistence/ProjectPersistencePolicy.hpp"

namespace ellindyer::project::persistence
{

class ProjectPersister
{
public:
    ProjectPersister() = delete;

    [[nodiscard]] static ellindyer::core::Result<void> Save(
        Project& project,
        const ProjectPersistencePolicy& policy);

    [[nodiscard]] static ellindyer::core::Result<void> Load(
        Project& project,
        const ProjectPersistencePolicy& policy);

    [[nodiscard]] static ellindyer::core::Result<void> SaveToRoot(
        Project& project,
        const std::filesystem::path& new_root,
        const ProjectPersistencePolicy& policy);

    [[nodiscard]] static bool HasOnDiskManifest(
        const std::filesystem::path& project_root);
};

} // namespace ellindyer::project::persistence
