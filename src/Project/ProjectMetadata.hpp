#pragma once

#include <cstdint>
#include <string>

namespace ellindyer::project
{

struct ProjectMetadata
{
    std::string  name;
    std::string  description;
    std::string  author;
    std::string  organization;
    std::string  world_name;
    std::string  world_description;
    std::uint32_t format_version = 1;
    std::string  created_utc;
    std::string  modified_utc;
    std::string  engine_target;
    std::string  project_version;
};

} // namespace ellindyer::project
