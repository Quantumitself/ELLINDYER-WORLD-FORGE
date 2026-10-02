#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Project/Project.hpp"
#include "Project/ProjectEvents.hpp"
#include "Project/ProjectMetadata.hpp"
#include "Project/Persistence/ProjectPersistencePolicy.hpp"

namespace ellindyer::project
{

struct ProjectCreateOptions
{
    std::filesystem::path root;
    ProjectMetadata        metadata;
    bool                   overwrite_existing = false;
};

struct ProjectOpenOptions
{
    std::filesystem::path root;
    bool                   require_manifest = true;
};

class ProjectManager
{
public:
    ProjectManager();
    ~ProjectManager();

    ProjectManager(const ProjectManager&) = delete;
    ProjectManager& operator=(const ProjectManager&) = delete;
    ProjectManager(ProjectManager&&) noexcept = delete;
    ProjectManager& operator=(ProjectManager&&) noexcept = delete;

    [[nodiscard]] ellindyer::core::Result<void> CreateProject(
        const ProjectCreateOptions& options);

    [[nodiscard]] ellindyer::core::Result<void> OpenProject(
        const ProjectOpenOptions& options);

    [[nodiscard]] ellindyer::core::Result<void> CloseProject();

    [[nodiscard]] ellindyer::core::Result<void> SaveProject();

    [[nodiscard]] ellindyer::core::Result<void> SaveProjectAs(
        const std::filesystem::path& new_root);

    [[nodiscard]] ellindyer::core::Result<void> RecoverProjectFromBackup();

    [[nodiscard]] bool HasProject() const noexcept;

    [[nodiscard]] bool IsModified() const noexcept;

    [[nodiscard]] bool HasRecoverableBackup() const noexcept;

    [[nodiscard]] Project* GetProject() noexcept;

    [[nodiscard]] const Project* GetProject() const noexcept;

    [[nodiscard]] const std::filesystem::path& GetProjectRoot() const noexcept;

    [[nodiscard]] const std::string& GetProjectName() const noexcept;

    [[nodiscard]] ProjectState GetProjectState() const noexcept;

    [[nodiscard]] const ellindyer::project::persistence::ProjectPersistencePolicy&
        GetPersistencePolicy() const noexcept;

    void SetPersistencePolicy(
        const ellindyer::project::persistence::ProjectPersistencePolicy& policy);

    void AddEventListener(ProjectEventListener listener);

    void ClearEventListeners();

    [[nodiscard]] const std::vector<std::filesystem::path>& GetRecentProjects() const noexcept;

    void AddRecentProject(const std::filesystem::path& root);

    void ClearRecentProjects();

private:
    void EmitEvent(ProjectEventKind kind, const std::string& detail);

    [[nodiscard]] ellindyer::core::Result<void> LoadProjectFromDisk(
        const ProjectLayout& layout,
        const ProjectMetadata& fallback_metadata);

    [[nodiscard]] ellindyer::core::Result<void> WriteManifestForCurrentProject();

    [[nodiscard]] static ProjectMetadata MakeMetadataForNewProject(
        const ProjectMetadata& input,
        const std::filesystem::path& root);

    std::unique_ptr<Project>                 project_;
    std::vector<ProjectEventListener>        listeners_;
    std::vector<std::filesystem::path>       recent_projects_;
    ellindyer::project::persistence::ProjectPersistencePolicy persistence_policy_{};
};

} // namespace ellindyer::project
