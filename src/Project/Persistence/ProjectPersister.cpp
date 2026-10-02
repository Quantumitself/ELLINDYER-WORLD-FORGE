#include "Project/Persistence/ProjectPersister.hpp"

#include <system_error>

#include "Core/ErrorCode.hpp"
#include "Core/FileHelpers.hpp"
#include "Core/LogMacros.hpp"
#include "Project/ProjectPaths.hpp"
#include "Project/Persistence/ProjectBackupManager.hpp"
#include "Project/Persistence/ProjectMetadataSanitizer.hpp"
#include "Project/Serialization/ProjectManifestCodec.hpp"
#include "Project/Serialization/ProjectManifestIO.hpp"
#include "Project/Serialization/ProjectTimeUtils.hpp"

namespace ellindyer::project::persistence
{

namespace
{

bool Exists(const std::filesystem::path& path)
{
    std::error_code ec;
    return std::filesystem::exists(path, ec) && !ec;
}

std::size_t FileSizeOrZero(const std::filesystem::path& path)
{
    std::error_code ec;
    const auto size = std::filesystem::file_size(path, ec);
    if (ec)
    {
        return 0;
    }
    return static_cast<std::size_t>(size);
}

} // namespace

ellindyer::core::Result<void> ProjectPersister::Save(
    Project& project,
    const ProjectPersistencePolicy& policy)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;
    using ellindyer::project::serialization::ProjectManifest;
    using ellindyer::project::serialization::ProjectManifestCodec;
    using ellindyer::project::serialization::ProjectManifestIO;
    using ellindyer::project::serialization::ProjectTimeUtils;

    if (!project.IsOpen()
        && project.GetState() != ProjectState::Initialized)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "Project is not in a saveable state."));
    }

    const ProjectLayout& layout = project.GetLayout();
    if (layout.manifest_file.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidProject,
                                      "Project manifest path is empty."));
    }

    Result<void> directories_ready = ProjectPaths::EnsureDirectoriesExist(layout);
    if (directories_ready.HasError())
    {
        return directories_ready;
    }

    ProjectMetadata& metadata = project.GetMutableMetadata();
    ProjectMetadataSanitizer::Apply(metadata, policy);

    metadata.modified_utc = ProjectTimeUtils::CurrentUtcTimestamp();
    if (metadata.created_utc.empty())
    {
        metadata.created_utc = metadata.modified_utc;
    }
    if (metadata.format_version == 0)
    {
        metadata.format_version = 1;
    }

    ProjectManifest manifest = ProjectManifestCodec::FromProject(project);

    Result<std::string> encoded = ProjectManifestCodec::EncodeToString(manifest);
    if (encoded.HasError())
    {
        return Result<void>(encoded.GetError());
    }

    if (encoded.Value().size() > policy.max_manifest_bytes)
    {
        return Result<void>(MakeError(
            ErrorCode::SerializationError,
            "Encoded manifest exceeds the configured maximum size."));
    }

    if (policy.create_backup_on_save && Exists(layout.manifest_file))
    {
        Result<void> backup_result =
            ProjectBackupManager::RotateBackups(layout.manifest_file, policy);
        if (backup_result.HasError())
        {
            ELLINDYER_LOG_WARNING("Backup rotation failed: " +
                                  backup_result.GetError().ToDiagnosticString());
        }
    }

    return ProjectManifestIO::WriteToFileAtomic(layout.manifest_file, manifest);
}

ellindyer::core::Result<void> ProjectPersister::Load(
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

    std::error_code exists_ec;
    if (!std::filesystem::exists(layout.manifest_file, exists_ec) || exists_ec)
    {
        return Result<void>(MakeError(
            ErrorCode::FileNotFound,
            "Project manifest not found: " + layout.manifest_file.generic_string()));
    }

    const std::size_t manifest_size = FileSizeOrZero(layout.manifest_file);
    if (manifest_size > policy.max_manifest_bytes)
    {
        return Result<void>(MakeError(
            ErrorCode::InvalidProject,
            "Project manifest exceeds the configured maximum size."));
    }

    Result<ProjectManifest> manifest_result =
        ProjectManifestIO::ReadFromFile(layout.manifest_file);
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

    Result<void> directories_ready = ProjectPaths::EnsureDirectoriesExist(layout);
    if (directories_ready.HasError())
    {
        return directories_ready;
    }

    project.SetState(ProjectState::Loaded);
    project.ClearModified();

    return Result<void>{};
}

ellindyer::core::Result<void> ProjectPersister::SaveToRoot(
    Project& project,
    const std::filesystem::path& new_root,
    const ProjectPersistencePolicy& policy)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (new_root.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Target project root is empty."));
    }

    std::error_code create_ec;
    std::filesystem::create_directories(new_root, create_ec);
    if (create_ec)
    {
        return Result<void>(MakeError(
            ErrorCode::IoError,
            "Failed to create target project root: " + create_ec.message()));
    }

    ProjectLayout new_layout = ProjectLayout::FromRoot(new_root);

    Result<void> directories_ready = ProjectPaths::EnsureDirectoriesExist(new_layout);
    if (directories_ready.HasError())
    {
        return directories_ready;
    }

    const ProjectLayout previous_layout = project.GetLayout();
    project.SetLayout(new_layout);

    Result<void> save_result = Save(project, policy);
    if (save_result.HasError())
    {
        project.SetLayout(previous_layout);
        return save_result;
    }

    project.ClearModified();
    return Result<void>{};
}

bool ProjectPersister::HasOnDiskManifest(const std::filesystem::path& project_root)
{
    if (project_root.empty())
    {
        return false;
    }

    const ProjectLayout layout = ProjectLayout::FromRoot(project_root);
    return Exists(layout.manifest_file);
}

} // namespace ellindyer::project::persistence
