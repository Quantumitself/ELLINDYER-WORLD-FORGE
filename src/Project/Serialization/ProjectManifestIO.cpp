#include "Project/Serialization/ProjectManifestIO.hpp"

#include <system_error>

#include "Core/ErrorCode.hpp"
#include "Core/FileHelpers.hpp"
#include "Project/Serialization/ProjectManifestCodec.hpp"

namespace ellindyer::project::serialization
{

ellindyer::core::Result<ProjectManifest> ProjectManifestIO::ReadFromFile(
    const std::filesystem::path& manifest_file)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (manifest_file.empty())
    {
        return Result<ProjectManifest>(MakeError(ErrorCode::InvalidArgument,
                                                 "Manifest file path is empty."));
    }

    std::error_code exists_ec;
    if (!std::filesystem::exists(manifest_file, exists_ec))
    {
        return Result<ProjectManifest>(MakeError(
            ErrorCode::FileNotFound,
            "Manifest file not found: " + manifest_file.string()));
    }

    Result<std::string> content = ellindyer::core::FileHelpers::ReadAllText(manifest_file);
    if (content.HasError())
    {
        return Result<ProjectManifest>(content.GetError());
    }

    return ProjectManifestCodec::DecodeFromString(content.Value());
}

ellindyer::core::Result<void> ProjectManifestIO::WriteToFile(
    const std::filesystem::path& manifest_file,
    const ProjectManifest& manifest)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (manifest_file.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Manifest file path is empty."));
    }

    Result<std::string> encoded = ProjectManifestCodec::EncodeToString(manifest);
    if (encoded.HasError())
    {
        return Result<void>(encoded.GetError());
    }

    return ellindyer::core::FileHelpers::WriteAllText(manifest_file, encoded.Value(), true);
}

ellindyer::core::Result<void> ProjectManifestIO::WriteToFileAtomic(
    const std::filesystem::path& manifest_file,
    const ProjectManifest& manifest)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (manifest_file.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Manifest file path is empty."));
    }

    Result<std::string> encoded = ProjectManifestCodec::EncodeToString(manifest);
    if (encoded.HasError())
    {
        return Result<void>(encoded.GetError());
    }

    return ellindyer::core::FileHelpers::WriteTextAtomically(manifest_file, encoded.Value());
}

} // namespace ellindyer::project::serialization
