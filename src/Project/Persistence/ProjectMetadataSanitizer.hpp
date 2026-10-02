#pragma once

#include <string>

#include "Project/ProjectMetadata.hpp"
#include "Project/Persistence/ProjectPersistencePolicy.hpp"

namespace ellindyer::project::persistence
{

class ProjectMetadataSanitizer
{
public:
    ProjectMetadataSanitizer() = delete;

    static void Apply(ProjectMetadata& metadata,
                      const ProjectPersistencePolicy& policy);

    static void TrimToLength(std::string& text, std::uint32_t max_length);

    static void RemoveControlCharacters(std::string& text);

    static void NormalizeLineEndings(std::string& text);
};

} // namespace ellindyer::project::persistence
