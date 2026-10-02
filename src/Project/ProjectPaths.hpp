#pragma once

#include <filesystem>
#include <string>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Project/ProjectLayout.hpp"

namespace ellindyer::project
{

class ProjectPaths
{
public:
    ProjectPaths() = delete;

    [[nodiscard]] static ellindyer::core::Result<void> EnsureDirectoriesExist(
        const ProjectLayout& layout);

    [[nodiscard]] static std::string Describe(const ProjectLayout& layout);

private:
    [[nodiscard]] static ellindyer::core::Result<void> EnsureSingleDirectory(
        const std::filesystem::path& directory,
        const char* label);
};

} // namespace ellindyer::project
