#pragma once

#include <string>

#include "Core/Identifier.hpp"

namespace ellindyer::project
{

using ProjectId = ellindyer::core::Identifier;

[[nodiscard]] inline ProjectId MakeProjectId()
{
    return ellindyer::core::Identifier::Generate();
}

[[nodiscard]] inline ProjectId MakeProjectIdFromName(const std::string& name)
{
    return ellindyer::core::MakeIdentifier("project:" + name);
}

} // namespace ellindyer::project
