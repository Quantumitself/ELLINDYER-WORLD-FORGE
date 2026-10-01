#pragma once

#include <system_error>

#include "Core/Error.hpp"
#include "Core/ErrorCode.hpp"

namespace ellindyer::core
{

[[nodiscard]] ErrorCode MapSystemErrorToErrorCode(const std::error_code& ec) noexcept;

[[nodiscard]] Error MakeFileError(const std::error_code& ec,
                                  std::string message_prefix);

[[nodiscard]] Error MakeFileError(const std::error_code& ec,
                                  std::string message_prefix,
                                  std::string path);

} // namespace ellindyer::core
