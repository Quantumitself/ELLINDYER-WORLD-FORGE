#include "Core/PathHelpers.hpp"

#include <algorithm>
#include <cctype>
#include <vector>

namespace ellindyer::core
{

namespace
{

[[nodiscard]] std::string ToLowerAscii(std::string_view text)
{
    std::string result;
    result.reserve(text.size());
    for (char c : text)
    {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return result;
}

[[nodiscard]] std::string NormalizeExtension(std::string_view extension)
{
    if (extension.empty())
    {
        return std::string{};
    }
    if (extension.front() == '.')
    {
        return std::string(extension);
    }
    return std::string(".") + std::string(extension);
}

[[nodiscard]] bool IsReservedFileNameCharacter(char c) noexcept
{
    switch (c)
    {
    case '<':
    case '>':
    case ':':
    case '"':
    case '/':
    case '\\':
    case '|':
    case '?':
    case '*':
        return true;
    default:
        return false;
    }
}

} // namespace

bool PathHelpers::HasExtension(const std::filesystem::path& path,
                                std::string_view extension)
{
    const std::string normalized = NormalizeExtension(extension);
    const std::string actual = ToLowerAscii(path.extension().string());
    return actual == ToLowerAscii(normalized);
}

std::string PathHelpers::GetExtensionLower(const std::filesystem::path& path)
{
    return ToLowerAscii(path.extension().string());
}

std::filesystem::path PathHelpers::WithExtension(const std::filesystem::path& path,
                                                 std::string_view extension)
{
    std::filesystem::path result = path;
    result.replace_extension(NormalizeExtension(extension));
    return result;
}

std::filesystem::path PathHelpers::WithoutExtension(const std::filesystem::path& path)
{
    std::filesystem::path result = path;
    result.replace_extension();
    return result;
}

std::string PathHelpers::GetFileNameWithoutExtension(const std::filesystem::path& path)
{
    return path.stem().string();
}

std::filesystem::path PathHelpers::SanitizeFileName(std::string_view name)
{
    std::string sanitized;
    sanitized.reserve(name.size());

    for (char c : name)
    {
        if (IsReservedFileNameCharacter(c))
        {
            sanitized.push_back('_');
            continue;
        }
        const unsigned char uc = static_cast<unsigned char>(c);
        if (uc < 32U)
        {
            sanitized.push_back('_');
            continue;
        }
        sanitized.push_back(c);
    }

    // Trim leading/trailing spaces and dots
    while (!sanitized.empty()
           && (sanitized.back() == ' ' || sanitized.back() == '.'))
    {
        sanitized.pop_back();
    }

    std::size_t start = 0;
    while (start < sanitized.size() && sanitized[start] == ' ')
    {
        ++start;
    }
    if (start > 0)
    {
        sanitized.erase(0, start);
    }

    if (sanitized.empty())
    {
        sanitized = "unnamed";
    }

    return std::filesystem::path(sanitized);
}

bool PathHelpers::IsWithinDirectory(const std::filesystem::path& path,
                                    const std::filesystem::path& directory)
{
    if (path.empty() || directory.empty())
    {
        return false;
    }

    std::error_code ec;
    const std::filesystem::path absolute_path = std::filesystem::absolute(path, ec);
    if (ec)
    {
        return false;
    }

    const std::filesystem::path absolute_directory = std::filesystem::absolute(directory, ec);
    if (ec)
    {
        return false;
    }

    const std::filesystem::path normalized_path = absolute_path.lexically_normal();
    const std::filesystem::path normalized_directory = absolute_directory.lexically_normal();

    auto path_it = normalized_path.begin();
    auto directory_it = normalized_directory.begin();
    for (; directory_it != normalized_directory.end(); ++directory_it, ++path_it)
    {
        if (path_it == normalized_path.end())
        {
            return false;
        }
        if (*path_it != *directory_it)
        {
            return false;
        }
    }

    return true;
}

std::string PathHelpers::JoinPathSegments(const std::vector<std::string>& segments)
{
    std::filesystem::path combined;
    for (const auto& segment : segments)
    {
        if (segment.empty())
        {
            continue;
        }
        combined /= segment;
    }
    return combined.generic_string();
}

} // namespace ellindyer::core
