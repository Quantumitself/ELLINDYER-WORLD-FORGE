#pragma once

#include <functional>
#include <string>

#include "Project/Project.hpp"

namespace ellindyer::project
{

enum class ProjectEventKind
{
    Created,
    Opened,
    Closed,
    Saved,
    Modified,
    Renamed,
    MetadataChanged
};

struct ProjectEvent
{
    ProjectEventKind kind = ProjectEventKind::Created;
    ProjectId        project_id;
    std::string      project_name;
    std::string      detail;
};

using ProjectEventListener = std::function<void(const ProjectEvent&)>;

} // namespace ellindyer::project
