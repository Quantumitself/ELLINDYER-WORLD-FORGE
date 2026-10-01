#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

#include "Core/Math/MathUtils.hpp"

namespace ellindyer::core::math
{

template <typename T>
struct Size
{
    static_assert(std::is_arithmetic_v<T>, "Size requires an arithmetic type.");

    T width  = static_cast<T>(0);
    T height = static_cast<T>(0);

    constexpr Size() noexcept = default;

    constexpr Size(T in_width, T in_height) noexcept
        : width(in_width)
        , height(in_height)
    {
    }

    [[nodiscard]] static constexpr Size Zero() noexcept
    {
        return Size(static_cast<T>(0), static_cast<T>(0));
    }

    [[nodiscard]] constexpr T Area() const noexcept
    {
        return width * height;
    }

    [[nodiscard]] constexpr bool IsEmpty() const noexcept
    {
        return width <= static_cast<T>(0) || height <= static_cast<T>(0);
    }

    [[nodiscard]] constexpr T AspectRatio() const noexcept
    {
        return height != static_cast<T>(0) ? width / height : static_cast<T>(0);
    }

    [[nodiscard]] constexpr bool operator==(const Size& other) const noexcept
    {
        return width == other.width && height == other.height;
    }

    [[nodiscard]] constexpr bool operator!=(const Size& other) const noexcept
    {
        return !(*this == other);
    }

    [[nodiscard]] std::string ToString() const
    {
        return std::to_string(width) + "x" + std::to_string(height);
    }
};

using Sizef   = Size<float>;
using Sized   = Size<double>;
using Sizei   = Size<int>;
using Sizeu   = Size<std::uint32_t>;

} // namespace ellindyer::core::math
