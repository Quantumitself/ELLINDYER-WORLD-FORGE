#pragma once

#include <string>
#include <vector>

#include "Project/Project.hpp"

namespace ellindyer::project
{

struct ProjectValidationIssue
{
    std::string message;
    std::string category;
    bool        fatal = false;
};

struct ProjectValidationResult
{
    std::vector<ProjectValidationIssue> issues;

    [[nodiscard]] bool HasErrors() const noexcept;
    [[nodiscard]] bool HasWarnings() const noexcept;
    [[nodiscard]] std::size_t ErrorCount() const noexcept;
    [[nodiscard]] std::size_t WarningCount() const noexcept;
};

class ProjectValidator
{
public:
    ProjectValidator() = delete;

    [[nodiscard]] static ProjectValidationResult ValidateMetadata(
        const ProjectMetadata& metadata);

    [[nodiscard]] static ProjectValidationResult ValidateLayout(
        const ProjectLayout& layout);

    [[nodiscard]] static ProjectValidationResult ValidateProject(
        const Project& project);
};

} // namespace ellindyer::project
