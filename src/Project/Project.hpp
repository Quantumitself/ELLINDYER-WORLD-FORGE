#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Project/ProjectIdentifier.hpp"
#include "Project/ProjectLayout.hpp"
#include "Project/ProjectMetadata.hpp"

namespace ellindyer::project
{

enum class ProjectState : std::uint8_t
{
    Empty,
    Initialized,
    Loaded,
    Modified
};

class Project
{
public:
    Project();
    explicit Project(const std::filesystem::path& root);
    ~Project();

    Project(const Project&) = delete;
    Project& operator=(const Project&) = delete;
    Project(Project&&) noexcept = default;
    Project& operator=(Project&&) noexcept = default;

    [[nodiscard]] static Project Create(const std::filesystem::path& root,
                                        const ProjectMetadata& metadata);

    [[nodiscard]] ProjectId GetId() const noexcept;

    void SetId(ProjectId id) noexcept;

    [[nodiscard]] const ProjectLayout& GetLayout() const noexcept;

    void SetLayout(const ProjectLayout& layout);

    [[nodiscard]] const ProjectMetadata& GetMetadata() const noexcept;

    void SetMetadata(const ProjectMetadata& metadata);

    [[nodiscard]] ProjectMetadata& GetMutableMetadata() noexcept;

    [[nodiscard]] ProjectState GetState() const noexcept;

    void SetState(ProjectState state) noexcept;

    [[nodiscard]] bool IsOpen() const noexcept;

    void MarkModified() noexcept;

    void ClearModified() noexcept;

    [[nodiscard]] bool IsModified() const noexcept;

    [[nodiscard]] std::string GetDisplayName() const;

private:
    ProjectId        id_;
    ProjectLayout    layout_;
    ProjectMetadata  metadata_;
    ProjectState     state_ = ProjectState::Empty;
    bool             modified_ = false;
};

} // namespace ellindyer::project
