#include "Project/Project.hpp"

#include <utility>

namespace ellindyer::project
{

Project::Project() = default;

Project::Project(const std::filesystem::path& root)
    : id_(MakeProjectId())
    , layout_(ProjectLayout::FromRoot(root))
{
    if (!root.empty())
    {
        state_ = ProjectState::Initialized;
    }
}

Project::~Project() = default;

Project Project::Create(const std::filesystem::path& root,
                        const ProjectMetadata& metadata)
{
    Project project{root};
    project.id_       = MakeProjectId();
    project.metadata_ = metadata;
    project.state_    = ProjectState::Initialized;
    project.modified_ = false;
    return project;
}

ProjectId Project::GetId() const noexcept
{
    return id_;
}

void Project::SetId(ProjectId id) noexcept
{
    id_ = id;
    modified_ = true;
}

const ProjectLayout& Project::GetLayout() const noexcept
{
    return layout_;
}

void Project::SetLayout(const ProjectLayout& layout)
{
    layout_ = layout;
    modified_ = true;
}

const ProjectMetadata& Project::GetMetadata() const noexcept
{
    return metadata_;
}

void Project::SetMetadata(const ProjectMetadata& metadata)
{
    metadata_ = metadata;
    modified_ = true;
}

ProjectMetadata& Project::GetMutableMetadata() noexcept
{
    modified_ = true;
    return metadata_;
}

ProjectState Project::GetState() const noexcept
{
    return state_;
}

void Project::SetState(ProjectState state) noexcept
{
    state_ = state;
}

bool Project::IsOpen() const noexcept
{
    return state_ == ProjectState::Loaded
        || state_ == ProjectState::Modified;
}

void Project::MarkModified() noexcept
{
    modified_ = true;
    if (state_ == ProjectState::Loaded)
    {
        state_ = ProjectState::Modified;
    }
}

void Project::ClearModified() noexcept
{
    modified_ = false;
    if (state_ == ProjectState::Modified)
    {
        state_ = ProjectState::Loaded;
    }
}

bool Project::IsModified() const noexcept
{
    return modified_;
}

std::string Project::GetDisplayName() const
{
    if (!metadata_.name.empty())
    {
        return metadata_.name;
    }
    if (!layout_.root.empty())
    {
        return layout_.root.filename().string();
    }
    return "Untitled Project";
}

} // namespace ellindyer::project
