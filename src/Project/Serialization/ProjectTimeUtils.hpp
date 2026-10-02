#pragma once

#include <string>

namespace ellindyer::project::serialization
{

class ProjectTimeUtils
{
public:
    ProjectTimeUtils() = delete;

    [[nodiscard]] static std::string CurrentUtcTimestamp();

    [[nodiscard]] static bool IsValidUtcTimestamp(const std::string& timestamp) noexcept;
};

} // namespace ellindyer::project::serialization
