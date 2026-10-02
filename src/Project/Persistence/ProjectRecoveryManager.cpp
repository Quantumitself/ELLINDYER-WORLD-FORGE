#include "Project/Persistence/ProjectRecoveryManager.hpp"

#include <system_error>

#include "Core/ErrorCode.hpp"
#include "Project/Persistence/ProjectBackupManager.hpp"
#include "Project/Persistence/ProjectPersister.hpp"
#include "Project/Persistence/ProjectMetadataSanitizer.hpp"
#include "Project/Serialization/ProjectManifestCodec.hpp"
#include "Project/Serialization/ProjectManifestIO.hpp"

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

bool ProjectRecoveryManager::CanRecover(const std::filesystem::path& project_root)
{
    if (project_root.empty())
    {
        return false;
    }

    const ProjectLayout layout = ProjectLayout::FromRoot(project_root);
    return ProjectBackupManager::HasBackup(layout.manifest_file);
}

ellindyer::core::Result<void> ProjectRecoveryManager::RecoverFromBackup(
    Project& project,
    const ProjectPersistencePolicy& policy)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;
    using ellindyer::project::serialization::ProjectManifest;
    using ellindyer::project::serialization::ProjectManifestCodec;
    using ellindyer::project::serialization::ProjectManifestIO;

    const ProjectLayout& layout = project.GetLayout();
    if (layout.manifest_file.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidProject,
                                      "Project manifest path is empty."));
    }

    const std::filesystem::path backup =
        ProjectBackupManager::LatestBackupPath(layout.manifest_file);

    if (backup.empty() || !Exists(backup))
    {
        return Result<void>(MakeError(ErrorCode::FileNotFound,
                                      "No backup is available for this project."));
    }

    Result<ProjectManifest> manifest_result =
        ProjectManifestIO::ReadFromFile(backup);
    if (manifest_result.HasError())
    {
        return Result<void>(manifest_result.GetError());
    }

    ProjectManifest manifest = manifest_result.Value();
    ProjectMetadataSanitizer::Apply(manifest.metadata, policy);

    Result<void> apply_result =
        ProjectManifestCodec::ApplyToProject(manifest, project);
    if (apply_result.HasError())
    {
        return apply_result;
    }

    return ProjectPersister::Save(project, policy);
}

ellindyer::core::Result<void> ProjectRecoveryManager::DiscardBackups(
    const std::filesystem::path& project_root,
    const ProjectPersistencePolicy& policy)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (project_root.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Project root is empty."));
    }

    const ProjectLayout layout = ProjectLayout::FromRoot(project_root);
    return ProjectBackupManager::DeleteAllBackups(layout.manifest_file, policy);
}

} // namespace ellindyer::project::persistence
