#include "Core/ErrorCode.hpp"

namespace ellindyer::core
{

std::string_view ToString(ErrorCode code) noexcept
{
    switch (code)
    {
    case ErrorCode::None:                 return std::string_view{"None"};

    case ErrorCode::Unknown:              return std::string_view{"Unknown"};
    case ErrorCode::NotSupported:         return std::string_view{"NotSupported"};
    case ErrorCode::NotImplemented:       return std::string_view{"NotImplemented"};
    case ErrorCode::InvalidArgument:      return std::string_view{"InvalidArgument"};
    case ErrorCode::InvalidState:         return std::string_view{"InvalidState"};
    case ErrorCode::OutOfRange:           return std::string_view{"OutOfRange"};
    case ErrorCode::AlreadyExists:        return std::string_view{"AlreadyExists"};
    case ErrorCode::NotFound:             return std::string_view{"NotFound"};

    case ErrorCode::FileNotFound:         return std::string_view{"FileNotFound"};
    case ErrorCode::FileExists:           return std::string_view{"FileExists"};
    case ErrorCode::AccessDenied:         return std::string_view{"AccessDenied"};
    case ErrorCode::DirectoryNotFound:    return std::string_view{"DirectoryNotFound"};
    case ErrorCode::PathTooLong:          return std::string_view{"PathTooLong"};
    case ErrorCode::IoError:              return std::string_view{"IoError"};
    case ErrorCode::ReadError:            return std::string_view{"ReadError"};
    case ErrorCode::WriteError:           return std::string_view{"WriteError"};
    case ErrorCode::PermissionDenied:     return std::string_view{"PermissionDenied"};
    case ErrorCode::DiskFull:             return std::string_view{"DiskFull"};

    case ErrorCode::InvalidData:          return std::string_view{"InvalidData"};
    case ErrorCode::InvalidProject:       return std::string_view{"InvalidProject"};
    case ErrorCode::InvalidSchema:        return std::string_view{"InvalidSchema"};
    case ErrorCode::InvalidEntity:        return std::string_view{"InvalidEntity"};
    case ErrorCode::InvalidRelationship:  return std::string_view{"InvalidRelationship"};
    case ErrorCode::InvalidIdentifier:    return std::string_view{"InvalidIdentifier"};
    case ErrorCode::SerializationError:   return std::string_view{"SerializationError"};
    case ErrorCode::DeserializationError: return std::string_view{"DeserializationError"};
    case ErrorCode::ParseError:           return std::string_view{"ParseError"};
    case ErrorCode::ValidationError:      return std::string_view{"ValidationError"};

    case ErrorCode::ResourceMissing:      return std::string_view{"ResourceMissing"};
    case ErrorCode::ResourceLoadFailed:   return std::string_view{"ResourceLoadFailed"};
    case ErrorCode::ResourceCorrupt:      return std::string_view{"ResourceCorrupt"};

    case ErrorCode::Timeout:              return std::string_view{"Timeout"};
    case ErrorCode::Cancelled:            return std::string_view{"Cancelled"};
    case ErrorCode::ConcurrencyConflict:  return std::string_view{"ConcurrencyConflict"};

    case ErrorCode::ConfigurationMissing: return std::string_view{"ConfigurationMissing"};
    case ErrorCode::ConfigurationInvalid: return std::string_view{"ConfigurationInvalid"};

    case ErrorCode::SystemError:          return std::string_view{"SystemError"};
    case ErrorCode::PlatformError:        return std::string_view{"PlatformError"};
    }

    return std::string_view{"Unknown"};
}

bool IsSuccess(ErrorCode code) noexcept
{
    return code == ErrorCode::None;
}

bool IsFailure(ErrorCode code) noexcept
{
    return code != ErrorCode::None;
}

std::string_view GetErrorCategory(ErrorCode code) noexcept
{
    switch (code)
    {
    case ErrorCode::None:
        return std::string_view{"none"};

    case ErrorCode::Unknown:
    case ErrorCode::NotSupported:
    case ErrorCode::NotImplemented:
    case ErrorCode::InvalidArgument:
    case ErrorCode::InvalidState:
    case ErrorCode::OutOfRange:
    case ErrorCode::AlreadyExists:
    case ErrorCode::NotFound:
        return std::string_view{"generic"};

    case ErrorCode::FileNotFound:
    case ErrorCode::FileExists:
    case ErrorCode::AccessDenied:
    case ErrorCode::DirectoryNotFound:
    case ErrorCode::PathTooLong:
    case ErrorCode::IoError:
    case ErrorCode::ReadError:
    case ErrorCode::WriteError:
    case ErrorCode::PermissionDenied:
    case ErrorCode::DiskFull:
        return std::string_view{"filesystem"};

    case ErrorCode::InvalidData:
    case ErrorCode::InvalidProject:
    case ErrorCode::InvalidSchema:
    case ErrorCode::InvalidEntity:
    case ErrorCode::InvalidRelationship:
    case ErrorCode::InvalidIdentifier:
    case ErrorCode::SerializationError:
    case ErrorCode::DeserializationError:
    case ErrorCode::ParseError:
    case ErrorCode::ValidationError:
        return std::string_view{"data"};

    case ErrorCode::ResourceMissing:
    case ErrorCode::ResourceLoadFailed:
    case ErrorCode::ResourceCorrupt:
        return std::string_view{"resource"};

    case ErrorCode::Timeout:
    case ErrorCode::Cancelled:
    case ErrorCode::ConcurrencyConflict:
        return std::string_view{"runtime"};

    case ErrorCode::ConfigurationMissing:
    case ErrorCode::ConfigurationInvalid:
        return std::string_view{"configuration"};

    case ErrorCode::SystemError:
    case ErrorCode::PlatformError:
        return std::string_view{"system"};
    }

    return std::string_view{"unknown"};
}

} // namespace ellindyer::core
