#include "Project/ProjectValidation.hpp"

#include <filesystem>

namespace ellindyer::project
{

bool ProjectValidationResult::HasErrors() const noexcept
{
    for (const ProjectValidationIssue& issue : issues)
    {
        if (issue.fatal)
        {
            return true;
        }
    }
    return false;
}

bool ProjectValidationResult::HasWarnings() const noexcept
{
    if (issues.empty())
    {
        return false;
    }
    for (const ProjectValidationIssue& issue : issues)
    {
        if (!issue.fatal)
        {
            return true;
        }
    }
    return false;
}

std::size_t ProjectValidationResult::ErrorCount() const noexcept
{
    std::size_t count = 0;
    for (const ProjectValidationIssue& issue : issues)
    {
        if (issue.fatal)
        {
            ++count;
        }
    }
    return count;
}

std::size_t ProjectValidationResult::WarningCount() const noexcept
{
    std::size_t count = 0;
    for (const ProjectValidationIssue& issue : issues)
    {
        if (!issue.fatal)
        {
            ++count;
        }
    }
    return count;
}

ProjectValidationResult ProjectValidator::ValidateMetadata(const ProjectMetadata& metadata)
{
    ProjectValidationResult result{};

    if (metadata.name.empty())
    {
        result.issues.push_back(ProjectValidationIssue{
            "Project name is empty.", "metadata", true});
    }

    if (metadata.name.size() > 128)
    {
        result.issues.push_back(ProjectValidationIssue{
            "Project name exceeds 128 characters.", "metadata", false});
    }

    if (metadata.format_version == 0)
    {
        result.issues.push_back(ProjectValidationIssue{
            "Project format version must be greater than zero.", "metadata", true});
    }

    if (metadata.world_name.empty())
    {
        result.issues.push_back(ProjectValidationIssue{
            "World name is empty.", "metadata", false});
    }

    return result;
}

ProjectValidationResult ProjectValidator::ValidateLayout(const ProjectLayout& layout)
{
    ProjectValidationResult result{};

    if (layout.root.empty())
    {
        result.issues.push_back(ProjectValidationIssue{
            "Project root directory is empty.", "layout", true});
        return result;
    }

    if (layout.manifest_file.empty())
    {
        result.issues.push_back(ProjectValidationIssue{
            "Project manifest path is empty.", "layout", true});
    }

    return result;
}

ProjectValidationResult ProjectValidator::ValidateProject(const Project& project)
{
    ProjectValidationResult result{};

    const ProjectValidationResult metadata_result =
        ValidateMetadata(project.GetMetadata());
    const ProjectValidationResult layout_result =
        ValidateLayout(project.GetLayout());

    for (const ProjectValidationIssue& issue : metadata_result.issues)
    {
        result.issues.push_back(issue);
    }
    for (const ProjectValidationIssue& issue : layout_result.issues)
    {
        result.issues.push_back(issue);
    }

    if (project.GetId().IsNil())
    {
        result.issues.push_back(ProjectValidationIssue{
            "Project identifier is nil.", "identity", true});
    }

    if (!project.IsOpen())
    {
        result.issues.push_back(ProjectValidationIssue{
            "Project is not in an open state.", "state", false});
    }

    return result;
}

} // namespace ellindyer::project
