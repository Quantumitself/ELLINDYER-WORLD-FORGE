#include "Core/Error.hpp"

#include <sstream>

namespace ellindyer::core
{

Error::Error(ErrorCode code, std::string message)
    : code_(code)
    , message_(std::move(message))
{
}

Error::Error(ErrorCode code, std::string_view message)
    : code_(code)
    , message_(message)
{
}

Error Error::Success() noexcept
{
    return Error{};
}

Error Error::FromCode(ErrorCode code)
{
    return Error(code, std::string(ellindyer::core::ToString(code)));
}

Error Error::FromMessage(ErrorCode code, std::string message)
{
    return Error(code, std::move(message));
}

Error Error::FromMessage(ErrorCode code, std::string_view message)
{
    return Error(code, message);
}

ErrorCode Error::Code() const noexcept
{
    return code_;
}

const std::string& Error::Message() const noexcept
{
    return message_;
}

bool Error::IsSuccess() const noexcept
{
    return code_ == ErrorCode::None;
}

bool Error::IsFailure() const noexcept
{
    return code_ != ErrorCode::None;
}

bool Error::HasMessage() const noexcept
{
    return !message_.empty();
}

void Error::Clear() noexcept
{
    code_ = ErrorCode::None;
    message_.clear();
}

std::string Error::ToString() const
{
    std::string result(ellindyer::core::ToString(code_));
    if (!message_.empty())
    {
        result.append(": ");
        result.append(message_);
    }
    return result;
}

std::string Error::ToDiagnosticString() const
{
    std::ostringstream stream;
    stream << '[' << GetErrorCategory(code_) << ']'
           << ' ' << ellindyer::core::ToString(code_)
           << " (" << static_cast<std::uint32_t>(code_) << ')';
    if (!message_.empty())
    {
        stream << " - " << message_;
    }
    return stream.str();
}

Error::operator bool() const noexcept
{
    return IsSuccess();
}

bool Error::operator==(const Error& other) const noexcept
{
    return code_ == other.code_ && message_ == other.message_;
}

bool Error::operator!=(const Error& other) const noexcept
{
    return !(*this == other);
}

Error MakeSuccess() noexcept
{
    return Error::Success();
}

Error MakeError(ErrorCode code)
{
    return Error::FromCode(code);
}

Error MakeError(ErrorCode code, std::string_view message)
{
    return Error::FromMessage(code, message);
}

} // namespace ellindyer::core
