#include "Project/Persistence/ProjectMetadataSanitizer.hpp"

#include <cctype>

namespace ellindyer::project::persistence
{

void ProjectMetadataSanitizer::TrimToLength(std::string& text, std::uint32_t max_length)
{
    if (max_length == 0)
    {
        text.clear();
        return;
    }
    if (text.size() > max_length)
    {
        text.resize(max_length);
        while (!text.empty()
               && (text.back() == ' ' || text.back() == '\t'
                   || text.back() == '\r' || text.back() == '\n'))
        {
            text.pop_back();
        }
    }
}

void ProjectMetadataSanitizer::RemoveControlCharacters(std::string& text)
{
    std::string cleaned;
    cleaned.reserve(text.size());
    for (char c : text)
    {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (uc == '\t' || uc == '\n' || uc == '\r')
        {
            cleaned.push_back(c);
            continue;
        }
        if (uc < 0x20U || uc == 0x7FU)
        {
            continue;
        }
        cleaned.push_back(c);
    }
    text = std::move(cleaned);
}

void ProjectMetadataSanitizer::NormalizeLineEndings(std::string& text)
{
    std::string normalized;
    normalized.reserve(text.size());
    for (std::size_t i = 0; i < text.size(); ++i)
    {
        const char c = text[i];
        if (c == '\r')
        {
            normalized.push_back('\n');
            if (i + 1 < text.size() && text[i + 1] == '\n')
            {
                ++i;
            }
            continue;
        }
        normalized.push_back(c);
    }
    text = std::move(normalized);
}

void ProjectMetadataSanitizer::Apply(ProjectMetadata& metadata,
                                     const ProjectPersistencePolicy& policy)
{
    const auto normalize_field = [&policy](std::string& field, std::uint32_t max_length)
    {
        RemoveControlCharacters(field);
        NormalizeLineEndings(field);
        TrimToLength(field, max_length);
    };

    normalize_field(metadata.name,              policy.max_metadata_name_len);
    normalize_field(metadata.description,       policy.max_metadata_desc_len);
    normalize_field(metadata.author,            policy.max_metadata_author_len);
    normalize_field(metadata.organization,      policy.max_metadata_org_len);
    normalize_field(metadata.world_name,        policy.max_metadata_world_len);
    normalize_field(metadata.world_description, policy.max_metadata_world_desc_len);
    normalize_field(metadata.engine_target,     policy.max_metadata_engine_len);
    normalize_field(metadata.project_version,   policy.max_metadata_version_len);
}

} // namespace ellindyer::project::persistence
