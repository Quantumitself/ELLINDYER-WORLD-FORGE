#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "Core/Error.hpp"
#include "Core/Result.hpp"

namespace ellindyer::core
{

class FileHelpers
{
public:
    FileHelpers() = delete;

    // Existence & type queries

    [[nodiscard]] static bool Exists(const std::filesystem::path& path);

    [[nodiscard]] static bool IsRegularFile(const std::filesystem::path& path);

    [[nodiscard]] static bool IsDirectory(const std::filesystem::path& path);

    [[nodiscard]] static bool IsEmpty(const std::filesystem::path& path);

    // Size & metadata

    [[nodiscard]] static Result<std::uintmax_t> GetFileSize(const std::filesystem::path& path);

    [[nodiscard]] static Result<std::filesystem::file_time_type> GetLastWriteTime(
        const std::filesystem::path& path);

    [[nodiscard]] static Result<std::filesystem::file_time_type> GetLastAccessTime(
        const std::filesystem::path& path);

    // Path normalization

    [[nodiscard]] static std::filesystem::path NormalizePath(const std::filesystem::path& path);

    [[nodiscard]] static std::filesystem::path MakeRelativeTo(
        const std::filesystem::path& path,
        const std::filesystem::path& base);

    [[nodiscard]] static std::filesystem::path MakeAbsolute(
        const std::filesystem::path& path);

    [[nodiscard]] static std::string NormalizeSeparators(std::string_view path_text);

    [[nodiscard]] static std::string ToGenericString(const std::filesystem::path& path);

    // Directory creation

    [[nodiscard]] static Result<void> CreateDirectoryIfMissing(
        const std::filesystem::path& directory);

    [[nodiscard]] static Result<void> CreateParentDirectoryIfMissing(
        const std::filesystem::path& file_path);

    // Reading & writing

    [[nodiscard]] static Result<std::string> ReadAllText(
        const std::filesystem::path& path);

    [[nodiscard]] static Result<std::vector<std::uint8_t>> ReadAllBytes(
        const std::filesystem::path& path);

    [[nodiscard]] static Result<void> WriteAllText(
        const std::filesystem::path& path,
        std::string_view text,
        bool truncate_existing = true);

    [[nodiscard]] static Result<void> WriteAllBytes(
        const std::filesystem::path& path,
        const std::vector<std::uint8_t>& bytes,
        bool truncate_existing = true);

    [[nodiscard]] static Result<void> AppendText(
        const std::filesystem::path& path,
        std::string_view text);

    // File operations

    [[nodiscard]] static Result<void> CopyFile(const std::filesystem::path& source,
                                               const std::filesystem::path& destination,
                                               bool overwrite_existing = false);

    [[nodiscard]] static Result<void> MoveFile(const std::filesystem::path& source,
                                               const std::filesystem::path& destination,
                                               bool overwrite_existing = false);

    [[nodiscard]] static Result<void> RemoveFile(const std::filesystem::path& path);

    [[nodiscard]] static Result<void> RemoveDirectoryRecursive(
        const std::filesystem::path& path);

    // Enumeration

    [[nodiscard]] static Result<std::vector<std::filesystem::path>> ListDirectory(
        const std::filesystem::path& directory,
        bool recursive = false);

    [[nodiscard]] static Result<std::vector<std::filesystem::path>> ListFiles(
        const std::filesystem::path& directory,
        bool recursive = false);

    [[nodiscard]] static Result<std::vector<std::filesystem::path>> ListSubdirectories(
        const std::filesystem::path& directory,
        bool recursive = false);

    [[nodiscard]] static Result<std::vector<std::filesystem::path>> FindByExtension(
        const std::filesystem::path& directory,
        std::string_view extension,
        bool recursive = false);

    // Temporary files

    [[nodiscard]] static Result<std::filesystem::path> CreateTemporaryFile(
        const std::filesystem::path& directory,
        std::string_view prefix,
        std::string_view extension);

    // Safe operations

    [[nodiscard]] static Result<void> WriteTextAtomically(
        const std::filesystem::path& path,
        std::string_view text);

    [[nodiscard]] static Result<void> WriteBytesAtomically(
        const std::filesystem::path& path,
        const std::vector<std::uint8_t>& bytes);
};

} // namespace ellindyer::core
