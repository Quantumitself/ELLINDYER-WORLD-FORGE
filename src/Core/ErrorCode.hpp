#pragma once

#include <cstdint>
#include <string_view>

namespace ellindyer::core
{

enum class ErrorCode : std::uint32_t
{
    None = 0,

    // Generic
    Unknown = 1,
    NotSupported,
    NotImplemented,
    InvalidArgument,
    InvalidState,
    OutOfRange,
    AlreadyExists,
    NotFound,

    // File system
    FileNotFound,
    FileExists,
    AccessDenied,
    DirectoryNotFound,
    PathTooLong,
    IoError,
    ReadError,
    WriteError,
    PermissionDenied,
    DiskFull,

    // Data / project
    InvalidData,
    InvalidProject,
    InvalidSchema,
    InvalidEntity,
    InvalidRelationship,
    InvalidIdentifier,
    SerializationError,
    DeserializationError,
    ParseError,
    ValidationError,

    // Resources
    ResourceMissing,
    ResourceLoadFailed,
    ResourceCorrupt,

    // Concurrency / runtime
    Timeout,
    Cancelled,
    ConcurrencyConflict,

    // Configuration
    ConfigurationMissing,
    ConfigurationInvalid,

    // System
    SystemError,
    PlatformError
};

[[nodiscard]] std::string_view ToString(ErrorCode code) noexcept;

[[nodiscard]] bool IsSuccess(ErrorCode code) noexcept;

[[nodiscard]] bool IsFailure(ErrorCode code) noexcept;

[[nodiscard]] std::string_view GetErrorCategory(ErrorCode code) noexcept;

} // namespace ellindyer::core
