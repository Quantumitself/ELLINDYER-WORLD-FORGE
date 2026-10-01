#include "Core/FileError.hpp"

#include <string>
#include <utility>

namespace ellindyer::core
{

ErrorCode MapSystemErrorToErrorCode(const std::error_code& ec) noexcept
{
    if (!ec)
    {
        return ErrorCode::None;
    }

    const std::errc condition = static_cast<std::errc>(ec.value());
    switch (condition)
    {
    case std::errc::no_such_file_or_directory:
        return ErrorCode::FileNotFound;

    case std::errc::file_exists:
        return ErrorCode::FileExists;

    case std::errc::permission_denied:
        return ErrorCode::PermissionDenied;

    case std::errc::not_a_directory:
        return ErrorCode::DirectoryNotFound;

    case std::errc::filename_too_long:
        return ErrorCode::PathTooLong;

    case std::errc::no_space_on_device:
        return ErrorCode::DiskFull;

    case std::errc::io_error:
        return ErrorCode::IoError;

    case std::errc::invalid_argument:
        return ErrorCode::InvalidArgument;

    case std::errc::result_out_of_range:
        return ErrorCode::OutOfRange;

    case std::errc::operation_canceled:
        return ErrorCode::Cancelled;

    case std::errc::timed_out:
        return ErrorCode::Timeout;

    default:
        break;
    }

    if (ec.category() == std::system_category())
    {
        return ErrorCode::SystemError;
    }

    return ErrorCode::IoError;
}

Error MakeFileError(const std::error_code& ec,
                    std::string message_prefix)
{
    const ErrorCode code = MapSystemErrorToErrorCode(ec);
    std::string message;
    message.reserve(message_prefix.size() + 2 + ec.message().size());
    message.append(message_prefix);
    if (!ec.message().empty())
    {
        message.append(": ");
        message.append(ec.message());
    }
    return Error(code, std::move(message));
}

Error MakeFileError(const std::error_code& ec,
                    std::string message_prefix,
                    std::string path)
{
    const ErrorCode code = MapSystemErrorToErrorCode(ec);
    std::string message;
    message.reserve(message_prefix.size() + 2 + path.size() + 2 + ec.message().size());
    message.append(message_prefix);
    message.append(" '");
    message.append(path);
    message.append("'");
    if (!ec.message().empty())
    {
        message.append(": ");
        message.append(ec.message());
    }
    return Error(code, std::move(message));
}

} // namespace ellindyer::core
