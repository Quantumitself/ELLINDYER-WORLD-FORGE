#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "Core/ErrorCode.hpp"

namespace ellindyer::core
{

class Error
{
public:
    Error() = default;

    Error(ErrorCode code, std::string message);

    Error(ErrorCode code, std::string_view message);

    [[nodiscard]] static Error Success() noexcept;

    [[nodiscard]] static Error FromCode(ErrorCode code);

    [[nodiscard]] static Error FromMessage(ErrorCode code, std::string message);

    [[nodiscard]] static Error FromMessage(ErrorCode code, std::string_view message);

    [[nodiscard]] ErrorCode Code() const noexcept;

    [[nodiscard]] const std::string& Message() const noexcept;

    [[nodiscard]] bool IsSuccess() const noexcept;

    [[nodiscard]] bool IsFailure() const noexcept;

    [[nodiscard]] bool HasMessage() const noexcept;

    void Clear() noexcept;

    [[nodiscard]] std::string ToString() const;

    [[nodiscard]] std::string ToDiagnosticString() const;

    [[nodiscard]] explicit operator bool() const noexcept;

    [[nodiscard]] bool operator==(const Error& other) const noexcept;

    [[nodiscard]] bool operator!=(const Error& other) const noexcept;

private:
    ErrorCode   code_ = ErrorCode::None;
    std::string message_;
};

[[nodiscard]] Error MakeSuccess() noexcept;

[[nodiscard]] Error MakeError(ErrorCode code);

[[nodiscard]] Error MakeError(ErrorCode code, std::string_view message);

} // namespace ellindyer::core
