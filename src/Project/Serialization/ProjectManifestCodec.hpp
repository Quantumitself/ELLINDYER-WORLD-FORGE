#pragma once

#include <string>
#include <string_view>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Project/Project.hpp"
#include "Project/Serialization/ProjectManifest.hpp"

namespace ellindyer::project::serialization
{

class ProjectManifestCodec
{
public:
    ProjectManifestCodec() = delete;

    // Convert between the in-memory Project and its serialized manifest form.

    [[nodiscard]] static ProjectManifest FromProject(const Project& project);

    [[nodiscard]] static ellindyer::core::Result<void> ApplyToProject(
        const ProjectManifest& manifest,
        Project& project);

    // Encode and decode to/from a string.

    [[nodiscard]] static ellindyer::core::Result<std::string> EncodeToString(
        const ProjectManifest& manifest);

    [[nodiscard]] static ellindyer::core::Result<ProjectManifest> DecodeFromString(
        std::string_view text);
};

} // namespace ellindyer::project::serialization
