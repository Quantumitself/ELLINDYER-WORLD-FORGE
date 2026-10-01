#include "Core/FileHelpers.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <system_error>

#include "Core/FileError.hpp"

namespace ellindyer::core
{

namespace
{

constexpr const char* kWindowsNativeSeparator = "\\";
constexpr const char* kGenericSeparator       = "/";

[[nodiscard]] std::uint64_t GenerateTemporarySuffix() noexcept
{
    static std::atomic<std::uint64_t> counter{0};

    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    std::uint64_t mixed = static_cast<std::uint64_t>(now);
    mixed ^= counter.fetch_add(1, std::memory_order_relaxed) * 0x9e3779b97f4a7c15ULL;
    mixed ^= mixed >> 33U;
    mixed *= 0xff51afd7ed558ccdULL;
    mixed ^= mixed >> 33U;

    return mixed;
}

[[nodiscard]] std::string ExtensionWithoutDot(std::string_view extension)
{
    if (!extension.empty() && extension.front() == '.')
    {
        extension.remove_prefix(1);
    }
    return std::string(extension);
}

} // namespace

bool FileHelpers::Exists(const std::filesystem::path& path)
{
    std::error_code ec;
    const bool exists = std::filesystem::exists(path, ec);
    return !ec && exists;
}

bool FileHelpers::IsRegularFile(const std::filesystem::path& path)
{
    std::error_code ec;
    const bool is_file = std::filesystem::is_regular_file(path, ec);
    return !ec && is_file;
}

bool FileHelpers::IsDirectory(const std::filesystem::path& path)
{
    std::error_code ec;
    const bool is_dir = std::filesystem::is_directory(path, ec);
    return !ec && is_dir;
}

bool FileHelpers::IsEmpty(const std::filesystem::path& path)
{
    std::error_code ec;
    const bool empty = std::filesystem::is_empty(path, ec);
    return !ec && empty;
}

Result<std::uintmax_t> FileHelpers::GetFileSize(const std::filesystem::path& path)
{
    std::error_code ec;
    const std::uintmax_t size = std::filesystem::file_size(path, ec);
    if (ec)
    {
        return Result<std::uintmax_t>(MakeFileError(ec, "Failed to query file size", path.string()));
    }
    return Result<std::uintmax_t>(size);
}

Result<std::filesystem::file_time_type> FileHelpers::GetLastWriteTime(
    const std::filesystem::path& path)
{
    std::error_code ec;
    const auto time = std::filesystem::last_write_time(path, ec);
    if (ec)
    {
        return Result<std::filesystem::file_time_type>(
            MakeFileError(ec, "Failed to query last write time", path.string()));
    }
    return Result<std::filesystem::file_time_type>(time);
}

Result<std::filesystem::file_time_type> FileHelpers::GetLastAccessTime(
    const std::filesystem::path& path)
{
    // std::filesystem does not expose a portable last-access-time API.
    // On Windows, the last write time is the closest stable analogue used here.
    return GetLastWriteTime(path);
}

std::filesystem::path FileHelpers::NormalizePath(const std::filesystem::path& path)
{
    if (path.empty())
    {
        return path;
    }

    std::error_code ec;
    std::filesystem::path normalized = std::filesystem::absolute(path, ec);
    if (ec)
    {
        normalized = path;
    }

    normalized = normalized.lexically_normal();
    return normalized;
}

std::filesystem::path FileHelpers::MakeRelativeTo(const std::filesystem::path& path,
                                                   const std::filesystem::path& base)
{
    if (path.empty() || base.empty())
    {
        return path;
    }

    std::error_code ec;
    const std::filesystem::path relative = std::filesystem::relative(path, base, ec);
    if (ec || relative.empty())
    {
        return path;
    }
    return relative;
}

std::filesystem::path FileHelpers::MakeAbsolute(const std::filesystem::path& path)
{
    std::error_code ec;
    const std::filesystem::path absolute = std::filesystem::absolute(path, ec);
    if (ec)
    {
        return path;
    }
    return absolute.lexically_normal();
}

std::string FileHelpers::NormalizeSeparators(std::string_view path_text)
{
    std::string result;
    result.reserve(path_text.size());
    for (char c : path_text)
    {
        if (c == '\\')
        {
            result.push_back('/');
        }
        else
        {
            result.push_back(c);
        }
    }
    return result;
}

std::string FileHelpers::ToGenericString(const std::filesystem::path& path)
{
    return path.generic_string();
}

Result<void> FileHelpers::CreateDirectoryIfMissing(const std::filesystem::path& directory)
{
    if (directory.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Directory path is empty."));
    }

    std::error_code ec;
    const bool exists = std::filesystem::exists(directory, ec);
    if (ec)
    {
        return Result<void>(MakeFileError(ec, "Failed to query directory", directory.string()));
    }
    if (exists)
    {
        if (std::filesystem::is_directory(directory, ec))
        {
            return Result<void>{};
        }
        return Result<void>(MakeError(ErrorCode::AlreadyExists,
                                      "Path exists but is not a directory: " +
                                          directory.string()));
    }

    std::filesystem::create_directories(directory, ec);
    if (ec)
    {
        return Result<void>(MakeFileError(ec, "Failed to create directory", directory.string()));
    }

    return Result<void>{};
}

Result<void> FileHelpers::CreateParentDirectoryIfMissing(const std::filesystem::path& file_path)
{
    const std::filesystem::path parent = file_path.parent_path();
    if (parent.empty())
    {
        return Result<void>{};
    }
    return CreateDirectoryIfMissing(parent);
}

Result<std::string> FileHelpers::ReadAllText(const std::filesystem::path& path)
{
    if (path.empty())
    {
        return Result<std::string>(MakeError(ErrorCode::InvalidArgument, "File path is empty."));
    }

    std::error_code ec;
    if (!std::filesystem::exists(path, ec))
    {
        return Result<std::string>(MakeError(ErrorCode::FileNotFound,
                                             "File not found: " + path.string()));
    }

    std::ifstream stream(path, std::ios::binary);
    if (!stream)
    {
        return Result<std::string>(
            MakeError(ErrorCode::ReadError, "Failed to open file for reading: " + path.string()));
    }

    std::string content;
    stream.seekg(0, std::ios::end);
    const std::streamoff size = stream.tellg();
    if (size > 0)
    {
        content.resize(static_cast<std::size_t>(size));
        stream.seekg(0, std::ios::beg);
        stream.read(content.data(), size);
    }
    stream.close();

    if (stream.fail() && !stream.eof())
    {
        return Result<std::string>(
            MakeError(ErrorCode::ReadError, "Failed to read file content: " + path.string()));
    }

    return Result<std::string>(std::move(content));
}

Result<std::vector<std::uint8_t>> FileHelpers::ReadAllBytes(const std::filesystem::path& path)
{
    if (path.empty())
    {
        return Result<std::vector<std::uint8_t>>(
            MakeError(ErrorCode::InvalidArgument, "File path is empty."));
    }

    std::error_code ec;
    if (!std::filesystem::exists(path, ec))
    {
        return Result<std::vector<std::uint8_t>>(
            MakeError(ErrorCode::FileNotFound, "File not found: " + path.string()));
    }

    std::ifstream stream(path, std::ios::binary);
    if (!stream)
    {
        return Result<std::vector<std::uint8_t>>(
            MakeError(ErrorCode::ReadError, "Failed to open file for reading: " + path.string()));
    }

    std::vector<std::uint8_t> bytes;
    stream.seekg(0, std::ios::end);
    const std::streamoff size = stream.tellg();
    if (size > 0)
    {
        bytes.resize(static_cast<std::size_t>(size));
        stream.seekg(0, std::ios::beg);
        stream.read(reinterpret_cast<char*>(bytes.data()), size);
    }
    stream.close();

    if (stream.fail() && !stream.eof())
    {
        return Result<std::vector<std::uint8_t>>(
            MakeError(ErrorCode::ReadError, "Failed to read file content: " + path.string()));
    }

    return Result<std::vector<std::uint8_t>>(std::move(bytes));
}

Result<void> FileHelpers::WriteAllText(const std::filesystem::path& path,
                                       std::string_view text,
                                       bool truncate_existing)
{
    if (path.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument, "File path is empty."));
    }

    const Result<void> parent_ready = CreateParentDirectoryIfMissing(path);
    if (parent_ready.HasError())
    {
        return parent_ready;
    }

    std::ios::openmode mode = std::ios::binary | std::ios::out;
    if (truncate_existing)
    {
        mode |= std::ios::trunc;
    }
    else
    {
        mode |= std::ios::app;
    }

    std::ofstream stream(path, mode);
    if (!stream)
    {
        return Result<void>(
            MakeError(ErrorCode::WriteError, "Failed to open file for writing: " + path.string()));
    }

    if (!text.empty())
    {
        stream.write(text.data(), static_cast<std::streamsize>(text.size()));
    }
    stream.flush();
    stream.close();

    if (stream.fail())
    {
        return Result<void>(
            MakeError(ErrorCode::WriteError, "Failed to write file content: " + path.string()));
    }

    return Result<void>{};
}

Result<void> FileHelpers::WriteAllBytes(const std::filesystem::path& path,
                                        const std::vector<std::uint8_t>& bytes,
                                        bool truncate_existing)
{
    if (path.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument, "File path is empty."));
    }

    const Result<void> parent_ready = CreateParentDirectoryIfMissing(path);
    if (parent_ready.HasError())
    {
        return parent_ready;
    }

    std::ios::openmode mode = std::ios::binary | std::ios::out;
    if (truncate_existing)
    {
        mode |= std::ios::trunc;
    }
    else
    {
        mode |= std::ios::app;
    }

    std::ofstream stream(path, mode);
    if (!stream)
    {
        return Result<void>(
            MakeError(ErrorCode::WriteError, "Failed to open file for writing: " + path.string()));
    }

    if (!bytes.empty())
    {
        stream.write(reinterpret_cast<const char*>(bytes.data()),
                     static_cast<std::streamsize>(bytes.size()));
    }
    stream.flush();
    stream.close();

    if (stream.fail())
    {
        return Result<void>(
            MakeError(ErrorCode::WriteError, "Failed to write file content: " + path.string()));
    }

    return Result<void>{};
}

Result<void> FileHelpers::AppendText(const std::filesystem::path& path,
                                     std::string_view text)
{
    return WriteAllText(path, text, false);
}

Result<void> FileHelpers::CopyFile(const std::filesystem::path& source,
                                   const std::filesystem::path& destination,
                                   bool overwrite_existing)
{
    std::error_code ec;
    if (!std::filesystem::exists(source, ec))
    {
        return Result<void>(MakeError(ErrorCode::FileNotFound,
                                      "Source file not found: " + source.string()));
    }

    const Result<void> parent_ready = CreateParentDirectoryIfMissing(destination);
    if (parent_ready.HasError())
    {
        return parent_ready;
    }

    const std::filesystem::copy_options options = overwrite_existing
        ? std::filesystem::copy_options::overwrite_existing
        : std::filesystem::copy_options::none;

    std::filesystem::copy_file(source, destination, options, ec);
    if (ec)
    {
        return Result<void>(MakeFileError(ec, "Failed to copy file", source.string()));
    }
    return Result<void>{};
}

Result<void> FileHelpers::MoveFile(const std::filesystem::path& source,
                                   const std::filesystem::path& destination,
                                   bool overwrite_existing)
{
    std::error_code ec;
    if (!std::filesystem::exists(source, ec))
    {
        return Result<void>(MakeError(ErrorCode::FileNotFound,
                                      "Source file not found: " + source.string()));
    }

    const Result<void> parent_ready = CreateParentDirectoryIfMissing(destination);
    if (parent_ready.HasError())
    {
        return parent_ready;
    }

    if (overwrite_existing)
    {
        std::error_code remove_ec;
        std::filesystem::remove(destination, remove_ec);
    }

    std::filesystem::rename(source, destination, ec);
    if (ec)
    {
        return Result<void>(MakeFileError(ec, "Failed to move file", source.string()));
    }
    return Result<void>{};
}

Result<void> FileHelpers::RemoveFile(const std::filesystem::path& path)
{
    std::error_code ec;
    const bool removed = std::filesystem::remove(path, ec);
    if (ec)
    {
        return Result<void>(MakeFileError(ec, "Failed to remove file", path.string()));
    }
    if (!removed)
    {
        return Result<void>(MakeError(ErrorCode::FileNotFound,
                                      "File not found: " + path.string()));
    }
    return Result<void>{};
}

Result<void> FileHelpers::RemoveDirectoryRecursive(const std::filesystem::path& path)
{
    std::error_code ec;
    if (!std::filesystem::exists(path, ec))
    {
        return Result<void>{};
    }

    std::filesystem::remove_all(path, ec);
    if (ec)
    {
        return Result<void>(MakeFileError(ec, "Failed to remove directory", path.string()));
    }
    return Result<void>{};
}

Result<std::vector<std::filesystem::path>> FileHelpers::ListDirectory(
    const std::filesystem::path& directory,
    bool recursive)
{
    std::vector<std::filesystem::path> entries;

    std::error_code ec;
    if (!std::filesystem::exists(directory, ec))
    {
        return Result<std::vector<std::filesystem::path>>(
            MakeError(ErrorCode::DirectoryNotFound,
                      "Directory not found: " + directory.string()));
    }
    if (!std::filesystem::is_directory(directory, ec))
    {
        return Result<std::vector<std::filesystem::path>>(
            MakeError(ErrorCode::InvalidArgument,
                      "Path is not a directory: " + directory.string()));
    }

    if (recursive)
    {
        std::filesystem::recursive_directory_iterator it(directory, ec);
        if (ec)
        {
            return Result<std::vector<std::filesystem::path>>(
                MakeFileError(ec, "Failed to enumerate directory", directory.string()));
        }
        for (const auto& entry : it)
        {
            entries.push_back(entry.path());
        }
    }
    else
    {
        std::filesystem::directory_iterator it(directory, ec);
        if (ec)
        {
            return Result<std::vector<std::filesystem::path>>(
                MakeFileError(ec, "Failed to enumerate directory", directory.string()));
        }
        for (const auto& entry : it)
        {
            entries.push_back(entry.path());
        }
    }

    return Result<std::vector<std::filesystem::path>>(std::move(entries));
}

Result<std::vector<std::filesystem::path>> FileHelpers::ListFiles(
    const std::filesystem::path& directory,
    bool recursive)
{
    Result<std::vector<std::filesystem::path>> all = ListDirectory(directory, recursive);
    if (all.HasError())
    {
        return all;
    }

    std::vector<std::filesystem::path> files;
    files.reserve(all.Value().size());

    std::error_code ec;
    for (const auto& entry : all.Value())
    {
        if (std::filesystem::is_regular_file(entry, ec))
        {
            files.push_back(entry);
        }
    }

    return Result<std::vector<std::filesystem::path>>(std::move(files));
}

Result<std::vector<std::filesystem::path>> FileHelpers::ListSubdirectories(
    const std::filesystem::path& directory,
    bool recursive)
{
    Result<std::vector<std::filesystem::path>> all = ListDirectory(directory, recursive);
    if (all.HasError())
    {
        return all;
    }

    std::vector<std::filesystem::path> directories;
    directories.reserve(all.Value().size());

    std::error_code ec;
    for (const auto& entry : all.Value())
    {
        if (std::filesystem::is_directory(entry, ec))
        {
            directories.push_back(entry);
        }
    }

    return Result<std::vector<std::filesystem::path>>(std::move(directories));
}

Result<std::vector<std::filesystem::path>> FileHelpers::FindByExtension(
    const std::filesystem::path& directory,
    std::string_view extension,
    bool recursive)
{
    Result<std::vector<std::filesystem::path>> files = ListFiles(directory, recursive);
    if (files.HasError())
    {
        return files;
    }

    const std::string normalized_extension = ExtensionWithoutDot(extension);
    const std::string dotted = "." + normalized_extension;

    std::vector<std::filesystem::path> matched;
    matched.reserve(files.Value().size());

    for (const auto& file : files.Value())
    {
        const std::string ext = file.extension().string();
        if (ext.size() != dotted.size())
        {
            continue;
        }

        bool same = true;
        for (std::size_t i = 0; i < ext.size(); ++i)
        {
            const char a = static_cast<char>(std::tolower(static_cast<unsigned char>(ext[i])));
            const char b = static_cast<char>(std::tolower(static_cast<unsigned char>(dotted[i])));
            if (a != b)
            {
                same = false;
                break;
            }
        }

        if (same)
        {
            matched.push_back(file);
        }
    }

    return Result<std::vector<std::filesystem::path>>(std::move(matched));
}

Result<std::filesystem::path> FileHelpers::CreateTemporaryFile(
    const std::filesystem::path& directory,
    std::string_view prefix,
    std::string_view extension)
{
    if (directory.empty())
    {
        return Result<std::filesystem::path>(
            MakeError(ErrorCode::InvalidArgument, "Temporary directory path is empty."));
    }

    const Result<void> dir_ready = CreateDirectoryIfMissing(directory);
    if (dir_ready.HasError())
    {
        return Result<std::filesystem::path>(dir_ready.GetError());
    }

    const std::string prefix_text = prefix.empty() ? "tmp" : std::string(prefix);
    const std::string extension_text = ExtensionWithoutDot(extension);

    for (int attempt = 0; attempt < 32; ++attempt)
    {
        const std::uint64_t suffix = GenerateTemporarySuffix();

        char name_buffer[64] = {};
        std::snprintf(name_buffer,
                      sizeof(name_buffer),
                      "%s_%016llx",
                      prefix_text.c_str(),
                      static_cast<unsigned long long>(suffix));

        std::filesystem::path candidate = directory / name_buffer;
        if (!extension_text.empty())
        {
            candidate += "." + extension_text;
        }

        std::error_code exists_ec;
        if (std::filesystem::exists(candidate, exists_ec))
        {
            continue;
        }

        std::ofstream stream(candidate, std::ios::binary | std::ios::out);
        if (!stream)
        {
            return Result<std::filesystem::path>(
                MakeError(ErrorCode::WriteError,
                          "Failed to create temporary file: " + candidate.string()));
        }
        stream.close();

        return Result<std::filesystem::path>(candidate);
    }

    return Result<std::filesystem::path>(
        MakeError(ErrorCode::IoError,
                  "Failed to allocate a unique temporary file name in: " + directory.string()));
}

Result<void> FileHelpers::WriteTextAtomically(const std::filesystem::path& path,
                                              std::string_view text)
{
    if (path.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument, "File path is empty."));
    }

    const Result<void> parent_ready = CreateParentDirectoryIfMissing(path);
    if (parent_ready.HasError())
    {
        return parent_ready;
    }

    Result<std::filesystem::path> temp_file =
        CreateTemporaryFile(path.parent_path(), "atomic", "tmp");
    if (temp_file.HasError())
    {
        return Result<void>(temp_file.GetError());
    }

    const Result<void> write_result = WriteAllText(temp_file.Value(), text, true);
    if (write_result.HasError())
    {
        std::error_code cleanup_ec;
        std::filesystem::remove(temp_file.Value(), cleanup_ec);
        return write_result;
    }

    std::error_code move_ec;
    std::filesystem::rename(temp_file.Value(), path, move_ec);
    if (move_ec)
    {
        std::error_code remove_ec;
        std::filesystem::remove(temp_file.Value(), remove_ec);
        return Result<void>(MakeFileError(move_ec, "Failed to commit atomic write", path.string()));
    }

    return Result<void>{};
}

Result<void> FileHelpers::WriteBytesAtomically(const std::filesystem::path& path,
                                               const std::vector<std::uint8_t>& bytes)
{
    if (path.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument, "File path is empty."));
    }

    const Result<void> parent_ready = CreateParentDirectoryIfMissing(path);
    if (parent_ready.HasError())
    {
        return parent_ready;
    }

    Result<std::filesystem::path> temp_file =
        CreateTemporaryFile(path.parent_path(), "atomic", "tmp");
    if (temp_file.HasError())
    {
        return Result<void>(temp_file.GetError());
    }

    const Result<void> write_result = WriteAllBytes(temp_file.Value(), bytes, true);
    if (write_result.HasError())
    {
        std::error_code cleanup_ec;
        std::filesystem::remove(temp_file.Value(), cleanup_ec);
        return write_result;
    }

    std::error_code move_ec;
    std::filesystem::rename(temp_file.Value(), path, move_ec);
    if (move_ec)
    {
        std::error_code remove_ec;
        std::filesystem::remove(temp_file.Value(), remove_ec);
        return Result<void>(MakeFileError(move_ec, "Failed to commit atomic write", path.string()));
    }

    return Result<void>{};
}

} // namespace ellindyer::core
