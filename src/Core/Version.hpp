#pragma once

#include <string>
#include <string_view>

namespace ellindyer::core
{

struct Version
{
    int major;
    int minor;
    int patch;

    [[nodiscard]] constexpr bool operator==(const Version& other) const noexcept
    {
        return major == other.major && minor == other.minor && patch == other.patch;
    }

    [[nodiscard]] constexpr bool operator!=(const Version& other) const noexcept
    {
        return !(*this == other);
    }

    [[nodiscard]] constexpr bool operator<(const Version& other) const noexcept
    {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }

    [[nodiscard]] constexpr bool operator>(const Version& other) const noexcept
    {
        return other < *this;
    }

    [[nodiscard]] constexpr bool operator<=(const Version& other) const noexcept
    {
        return !(other < *this);
    }

    [[nodiscard]] constexpr bool operator>=(const Version& other) const noexcept
    {
        return !(*this < other);
    }
};

[[nodiscard]] Version GetApplicationVersion() noexcept;

[[nodiscard]] std::string GetApplicationVersionString();

[[nodiscard]] std::string_view GetApplicationName() noexcept;

[[nodiscard]] std::string_view GetApplicationTagline() noexcept;

[[nodiscard]] std::string_view GetApplicationOrganization() noexcept;

} // namespace ellindyer::core
