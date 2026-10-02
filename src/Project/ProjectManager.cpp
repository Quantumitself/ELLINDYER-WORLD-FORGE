#include "Project/ProjectManager.hpp"

#include <algorithm>
#include <system_error>
#include <utility>

#include "Core/ErrorCode.hpp"
#include "Core/FileHelpers.hpp"
#include "Core/LogMacros.hpp"
#include "Project/ProjectPaths.hpp"
#include "Project/ProjectValidation.hpp"
#include "Project/Persistence/ProjectPersister.hpp"
#include "Project/Persistence/ProjectRecoveryManager.hpp"
#include "Project/Serialization/ProjectManifestCodec.hpp"
#include "Project/Serialization/ProjectManifestIO.hpp"
#include "Project/Serialization/ProjectTimeUtils.hpp"

namespace ellindyer::project
{

namespace
{

constexpr std::size_t kMaxRecentProjects = 16;
constexpr std::size_t kMaximumProjectNameLength = 128;

bool IsRootEmpty(const std::filesystem::path& root)
{
    std::error_code ec;
    if (!std::filesystem::exists(root, ec) || ec)
    {
        return true;
    }
    if (!std::filesystem::is_directory(root, ec) || ec)
    {
        return false;
    }
    return std::filesystem::is_empty(root, ec) && !ec;
}

} // namespace

ProjectManager::ProjectManager()
    : persistence_policy_(ellindyer::project::persistence::ProjectPersistencePolicy::Default())
{
}

ProjectManager::~ProjectManager() = default;

ellindyer::core::Result<void> ProjectManager::CreateProject(
    const ProjectCreateOptions& options)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;
    using ellindyer::project::persistence::ProjectPersister;

    if (options.root.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Project root directory is empty."));
    }

    ProjectMetadata metadata = MakeMetadataForNewProject(options.metadata,
                                                         options.root);

    const ProjectValidationResult metadata_validation =
        ProjectValidator::ValidateMetadata(metadata);
    if (metadata_validation.HasErrors())
    {
        std::string message = "Project metadata invalid: ";
        bool first = true;
        for (const ProjectValidationIssue& issue : metadata_validation.issues)
        {
            if (!issue.fatal)
            {
                continue;
            }
            if (!first)
            {
                message.append("; ");
            }
            message.append(issue.message);
            first = false;
        }
        return Result<void>(MakeError(ErrorCode::InvalidProject, message));
    }

    std::error_code exists_ec;
    const bool root_exists = std::filesystem::exists(options.root, exists_ec) && !exists_ec;

    if (root_exists && !options.overwrite_existing)
    {
        if (!IsRootEmpty(options.root))
        {
            return Result<void>(MakeError(
                ErrorCode::AlreadyExists,
                "Project root is not empty: " + options.root.generic_string()));
        }
    }

    std::error_code create_ec;
    std::filesystem::create_directories(options.root, create_ec);
    if (create_ec)
    {
        return Result<void>(MakeError(
            ErrorCode::IoError,
            "Failed to create project root: " + create_ec.message()));
    }

    ProjectLayout layout = ProjectLayout::FromRoot(options.root);

    Result<void> directories_ready = ProjectPaths::EnsureDirectoriesExist(layout);
    if (directories_ready.HasError())
    {
        return directories_ready;
    }

    project_ = std::make_unique<Project>(options.root);
    project_->SetId(MakeProjectId());
    project_->SetMetadata(metadata);
    project_->SetState(ProjectState::Initialized);
    project_->ClearModified();

    Result<void> save_result = ProjectPersister::Save(*project_, persistence_policy_);
    if (save_result.HasError())
    {
        project_.reset();
        return save_result;
    }

    project_->SetState(ProjectState::Loaded);
    project_->ClearModified();

    AddRecentProject(options.root);
    EmitEvent(ProjectEventKind::Created, layout.root.generic_string());

    ELLINDYER_LOG_INFO("Project created: " + project_->GetDisplayName());
    return Result<void>{};
}

ellindyer::core::Result<void> ProjectManager::OpenProject(
    const ProjectOpenOptions& options)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (options.root.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Project root directory is empty."));
    }

    std::error_code exists_ec;
    if (!std::filesystem::exists(options.root, exists_ec) || exists_ec)
    {
        return Result<void>(MakeError(
            ErrorCode::DirectoryNotFound,
            "Project root does not exist: " + options.root.generic_string()));
    }

    if (!std::filesystem::is_directory(options.root, exists_ec) || exists_ec)
    {
        return Result<void>(MakeError(
            ErrorCode::InvalidProject,
            "Project root is not a directory: " + options.root.generic_string()));
    }

    ProjectLayout layout = ProjectLayout::FromRoot(options.root);

    if (options.require_manifest)
    {
        std::error_code manifest_ec;
        if (!std::filesystem::exists(layout.manifest_file, manifest_ec) || manifest_ec)
        {
            return Result<void>(MakeError(
                ErrorCode::FileNotFound,
                "Project manifest not found: " + layout.manifest_file.generic_string()));
        }
    }

    ProjectMetadata fallback_metadata{};
    fallback_metadata.name = options.root.filename().string();
    fallback_metadata.format_version = 1;

    Result<void> load_result = LoadProjectFromDisk(layout, fallback_metadata);
    if (load_result.HasError())
    {
        return load_result;
    }

    AddRecentProject(options.root);
    EmitEvent(ProjectEventKind::Opened, layout.root.generic_string());

    ELLINDYER_LOG_INFO("Project opened: " + project_->GetDisplayName());
    return Result<void>{};
}

ellindyer::core::Result<void> ProjectManager::CloseProject()
{
    using ellindyer::core::Result;

    if (!project_)
    {
        return Result<void>{};
    }

    const std::string project_name = project_->GetDisplayName();

    EmitEvent(ProjectEventKind::Closed, project_->GetLayout().root.generic_string());

    project_.reset();

    ELLINDYER_LOG_INFO("Project closed: " + project_name);
    return Result<void>{};
}

ellindyer::core::Result<void> ProjectManager::SaveProject()
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;
    using ellindyer::project::persistence::ProjectPersister;

    if (!project_)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "No project is open."));
    }

    Result<void> save_result = ProjectPersister::Save(*project_, persistence_policy_);
    if (save_result.HasError())
    {
        return save_result;
    }

    project_->ClearModified();
    EmitEvent(ProjectEventKind::Saved, project_->GetLayout().root.generic_string());

    ELLINDYER_LOG_INFO("Project saved: " + project_->GetDisplayName());
    return Result<void>{};
}

ellindyer::core::Result<void> ProjectManager::SaveProjectAs(
    const std::filesystem::path& new_root)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;
    using ellindyer::project::persistence::ProjectPersister;

    if (!project_)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "No project is open."));
    }

    if (new_root.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Target project root is empty."));
    }

    std::error_code exists_ec;
    const bool root_exists = std::filesystem::exists(new_root, exists_ec) && !exists_ec;

    if (root_exists && !IsRootEmpty(new_root))
    {
        return Result<void>(MakeError(
            ErrorCode::AlreadyExists,
            "Target project root is not empty: " + new_root.generic_string()));
    }

    Result<void> save_result = ProjectPersister::SaveToRoot(
        *project_, new_root, persistence_policy_);
    if (save_result.HasError())
    {
        return save_result;
    }

    project_->ClearModified();
    AddRecentProject(new_root);
    EmitEvent(ProjectEventKind::Saved, new_root.generic_string());

    ELLINDYER_LOG_INFO("Project saved as: " + new_root.generic_string());
    return Result<void>{};
}

ellindyer::core::Result<void> ProjectManager::RecoverProjectFromBackup()
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;
    using ellindyer::project::persistence::ProjectRecoveryManager;

    if (!project_)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "No project is open."));
    }

    Result<void> recover_result =
        ProjectRecoveryManager::RecoverFromBackup(*project_, persistence_policy_);
    if (recover_result.HasError())
    {
        return recover_result;
    }

    project_->ClearModified();
    EmitEvent(ProjectEventKind::Opened, project_->GetLayout().root.generic_string());

    ELLINDYER_LOG_INFO("Project recovered from backup: " + project_->GetDisplayName());
    return Result<void>{};
}

bool ProjectManager::HasProject() const noexcept
{
    return project_ != nullptr;
}

bool ProjectManager::IsModified() const noexcept
{
    return project_ != nullptr && project_->IsModified();
}

bool ProjectManager::HasRecoverableBackup() const noexcept
{
    if (!project_)
    {
        return false;
    }
    return ellindyer::project::persistence::ProjectRecoveryManager::CanRecover(
        project_->GetLayout().root);
}

Project* ProjectManager::GetProject() noexcept
{
    return project_.get();
}

const Project* ProjectManager::GetProject() const noexcept
{
    return project_.get();
}

const std::filesystem::path& ProjectManager::GetProjectRoot() const noexcept
{
    static const std::filesystem::path empty_path{};
    if (!project_)
    {
        return empty_path;
    }
    return project_->GetLayout().root;
}

const std::string& ProjectManager::GetProjectName() const noexcept
{
    static const std::string empty_name{};
    if (!project_)
    {
        return empty_name;
    }
    return project_->GetMetadata().name;
}

ProjectState ProjectManager::GetProjectState() const noexcept
{
    if (!project_)
    {
        return ProjectState::Empty;
    }
    return project_->GetState();
}

const ellindyer::project::persistence::ProjectPersistencePolicy&
ProjectManager::GetPersistencePolicy() const noexcept
{
    return persistence_policy_;
}

void ProjectManager::SetPersistencePolicy(
    const ellindyer::project::persistence::ProjectPersistencePolicy& policy)
{
    persistence_policy_ = policy;
}

void ProjectManager::AddEventListener(ProjectEventListener listener)
{
    if (!listener)
    {
        return;
    }
    listeners_.push_back(std::move(listener));
}

void ProjectManager::ClearEventListeners()
{
    listeners_.clear();
}

const std::vector<std::filesystem::path>& ProjectManager::GetRecentProjects() const noexcept
{
    return recent_projects_;
}

void ProjectManager::AddRecentProject(const std::filesystem::path& root)
{
    if (root.empty())
    {
        return;
    }

    const std::filesystem::path normalized = root.lexically_normal();

    recent_projects_.erase(
        std::remove_if(recent_projects_.begin(),
                       recent_projects_.end(),
                       [&normalized](const std::filesystem::path& entry)
                       {
                           return entry.lexically_normal() == normalized;
                       }),
        recent_projects_.end());

    recent_projects_.insert(recent_projects_.begin(), normalized);

    if (recent_projects_.size() > kMaxRecentProjects)
    {
        recent_projects_.resize(kMaxRecentProjects);
    }
}

void ProjectManager::ClearRecentProjects()
{
    recent_projects_.clear();
}

void ProjectManager::EmitEvent(ProjectEventKind kind, const std::string& detail)
{
    ProjectEvent event{};
    event.kind         = kind;
    event.detail       = detail;
    event.project_id   = project_ != nullptr ? project_->GetId() : ProjectId{};
    event.project_name = project_ != nullptr
        ? project_->GetDisplayName()
        : std::string{};

    for (ProjectEventListener& listener : listeners_)
    {
        if (listener)
        {
            listener(event);
        }
    }
}

ellindyer::core::Result<void> ProjectManager::LoadProjectFromDisk(
    const ProjectLayout& layout,
    const ProjectMetadata& fallback_metadata)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;
    using ellindyer::project::persistence::ProjectPersister;

    project_.reset();

    project_ = std::make_unique<Project>(layout.root);

    std::error_code manifest_ec;
    const bool has_manifest =
        std::filesystem::exists(layout.manifest_file, manifest_ec) && !manifest_ec;

    if (has_manifest)
    {
        Result<void> load_result = ProjectPersister::Load(*project_, persistence_policy_);
        if (load_result.HasError())
        {
            project_.reset();
            return load_result;
        }
    }
    else
    {
        project_->SetId(MakeProjectId());
        project_->SetMetadata(fallback_metadata);
        project_->SetState(ProjectState::Loaded);
        project_->ClearModified();
    }

    Result<void> directories_ready = ProjectPaths::EnsureDirectoriesExist(layout);
    if (directories_ready.HasError())
    {
        project_.reset();
        return directories_ready;
    }

    return Result<void>{};
}

ellindyer::core::Result<void> ProjectManager::WriteManifestForCurrentProject()
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;
    using ellindyer::project::persistence::ProjectPersister;

    if (!project_)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "No project is open."));
    }

    return ProjectPersister::Save(*project_, persistence_policy_);
}

ProjectMetadata ProjectManager::MakeMetadataForNewProject(
    const ProjectMetadata& input,
    const std::filesystem::path& root)
{
    using ellindyer::project::serialization::ProjectTimeUtils;

    ProjectMetadata metadata = input;

    if (metadata.name.empty())
    {
        metadata.name = root.filename().string();
        if (metadata.name.empty())
        {
            metadata.name = "Untitled Project";
        }
    }

    if (metadata.name.size() > kMaximumProjectNameLength)
    {
        metadata.name.resize(kMaximumProjectNameLength);
    }

    if (metadata.format_version == 0)
    {
        metadata.format_version = 1;
    }

    const std::string now = ProjectTimeUtils::CurrentUtcTimestamp();
    if (metadata.created_utc.empty())
    {
        metadata.created_utc = now;
    }
    if (metadata.modified_utc.empty())
    {
        metadata.modified_utc = now;
    }

    if (metadata.world_name.empty())
    {
        metadata.world_name = metadata.name;
    }

    return metadata;
}

} // namespace ellindyer::project
