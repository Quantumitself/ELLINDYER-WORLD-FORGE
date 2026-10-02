#include "Project/Persistence/ProjectPersistencePolicy.hpp"

namespace ellindyer::project::persistence
{

const ProjectPersistencePolicy& ProjectPersistencePolicy::Default() noexcept
{
    static const ProjectPersistencePolicy instance{};
    return instance;
}

std::filesystem::path ProjectPersistencePolicy::BackupPathFor(
    const std::filesystem::path& manifest_file,
    std::uint32_t slot_index)
{
    if (manifest_file.empty())
    {
        return {};
    }

    std::filesystem::path backup = manifest_file;
    backup += ".bak";

    if (slot_index > 0)
    {
        backup += "." + std::to_string(slot_index);
    }

    return backup;
}

} // namespace ellindyer::project::persistence
