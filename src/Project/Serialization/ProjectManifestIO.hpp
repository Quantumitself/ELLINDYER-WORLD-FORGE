#pragma once

#include <filesystem>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Project/Serialization/ProjectManifest.hpp"

namespace ellindyer::project::serialization
{

class ProjectManifestIO
{
public:
    ProjectManifestIO() = delete;

    [[nodiscard]] static ellindyer::core::Result<ProjectManifest> ReadFromFile(
        const std::filesystem::path& manifest_file);

    [[nodiscard]] static ellindyer::core::Result<void> WriteToFile(
        const std::filesystem::path& manifest_file,
        const ProjectManifest& manifest);

    [[nodiscard]] static ellindyer::core::Result<void> WriteToFileAtomic(
        const std::filesystem::path& manifest_file,
        const ProjectManifest& manifest);
};

} // namespace ellindyer::project::serialization
