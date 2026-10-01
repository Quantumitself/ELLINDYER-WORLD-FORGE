#pragma once

#include <string>
#include <type_traits>

#include "Core/Math/MathConstants.hpp"
#include "Core/Math/MathUtils.hpp"
#include "Core/Math/Vec2.hpp"

namespace ellindyer::core::math
{

template <typename T>
struct Rect
{
    static_assert(std::is_arithmetic_v<T>, "Rect requires an arithmetic type.");

    T x      = static_cast<T>(0);
    T y      = static_cast<T>(0);
    T width  = static_cast<T>(0);
    T height = static_cast<T>(0);

    constexpr Rect() noexcept = default;

    constexpr Rect(T in_x, T in_y, T in_width, T in_height) noexcept
        : x(in_x)
        , y(in_y)
        , width(in_width)
        , height(in_height)
    {
    }

    template <typename U>
    constexpr explicit Rect(const Rect<U>& other) noexcept
        : x(static_cast<T>(other.x))
        , y(static_cast<T>(other.y))
        , width(static_cast<T>(other.width))
        , height(static_cast<T>(other.height))
    {
    }

    [[nodiscard]] static constexpr Rect Zero() noexcept
    {
        return Rect(static_cast<T>(0), static_cast<T>(0),
                    static_cast<T>(0), static_cast<T>(0));
    }

    [[nodiscard]] static Rect FromMinMax(const Vec2<T>& minimum, const Vec2<T>& maximum) noexcept
    {
        return Rect(minimum.x, minimum.y,
                    maximum.x - minimum.x,
                    maximum.y - minimum.y);
    }

    [[nodiscard]] static Rect FromCorners(const Vec2<T>& first, const Vec2<T>& second) noexcept
    {
        const T min_x = first.x < second.x ? first.x : second.x;
        const T min_y = first.y < second.y ? first.y : second.y;
        const T max_x = first.x > second.x ? first.x : second.x;
        const T max_y = first.y > second.y ? first.y : second.y;
        return Rect(min_x, min_y, max_x - min_x, max_y - min_y);
    }

    [[nodiscard]] constexpr T Left() const noexcept   { return x; }
    [[nodiscard]] constexpr T Top() const noexcept    { return y; }
    [[nodiscard]] constexpr T Right() const noexcept  { return x + width; }
    [[nodiscard]] constexpr T Bottom() const noexcept { return y + height; }

    [[nodiscard]] constexpr T MinX() const noexcept { return x; }
    [[nodiscard]] constexpr T MinY() const noexcept { return y; }
    [[nodiscard]] constexpr T MaxX() const noexcept { return x + width; }
    [[nodiscard]] constexpr T MaxY() const noexcept { return y + height; }

    [[nodiscard]] constexpr Vec2<T> Position() const noexcept
    {
        return Vec2<T>(x, y);
    }

    [[nodiscard]] constexpr Vec2<T> Size() const noexcept
    {
        return Vec2<T>(width, height);
    }

    [[nodiscard]] constexpr Vec2<T> Center() const noexcept
    {
        return Vec2<T>(x + width * static_cast<T>(0.5),
                       y + height * static_cast<T>(0.5));
    }

    [[nodiscard]] constexpr Vec2<T> TopLeft() const noexcept
    {
        return Vec2<T>(Left(), Top());
    }

    [[nodiscard]] constexpr Vec2<T> TopRight() const noexcept
    {
        return Vec2<T>(Right(), Top());
    }

    [[nodiscard]] constexpr Vec2<T> BottomLeft() const noexcept
    {
        return Vec2<T>(Left(), Bottom());
    }

    [[nodiscard]] constexpr Vec2<T> BottomRight() const noexcept
    {
        return Vec2<T>(Right(), Bottom());
    }

    [[nodiscard]] constexpr T Area() const noexcept
    {
        return width * height;
    }

    [[nodiscard]] constexpr T Perimeter() const noexcept
    {
        return static_cast<T>(2) * (width + height);
    }

    [[nodiscard]] constexpr bool IsEmpty() const noexcept
    {
        return width <= static_cast<T>(0) || height <= static_cast<T>(0);
    }

    [[nodiscard]] constexpr bool IsValid() const noexcept
    {
        return width >= static_cast<T>(0) && height >= static_cast<T>(0);
    }

    [[nodiscard]] constexpr bool Contains(const Vec2<T>& point) const noexcept
    {
        return point.x >= Left() && point.x <= Right()
            && point.y >= Top()  && point.y <= Bottom();
    }

    [[nodiscard]] constexpr bool Contains(T point_x, T point_y) const noexcept
    {
        return Contains(Vec2<T>(point_x, point_y));
    }

    [[nodiscard]] constexpr bool Contains(const Rect& other) const noexcept
    {
        return other.Left()   >= Left()
            && other.Right()  <= Right()
            && other.Top()    >= Top()
            && other.Bottom() <= Bottom();
    }

    [[nodiscard]] constexpr bool Intersects(const Rect& other) const noexcept
    {
        return !(other.Left()   > Right()
              || other.Right()  < Left()
              || other.Top()    > Bottom()
              || other.Bottom() < Top());
    }

    [[nodiscard]] constexpr Rect Intersection(const Rect& other) const noexcept
    {
        if (!Intersects(other))
        {
            return Zero();
        }

        const T min_x = Left()   > other.Left()   ? Left()   : other.Left();
        const T min_y = Top()    > other.Top()    ? Top()    : other.Top();
        const T max_x = Right()  < other.Right()  ? Right()  : other.Right();
        const T max_y = Bottom() < other.Bottom() ? Bottom() : other.Bottom();

        return Rect(min_x, min_y, max_x - min_x, max_y - min_y);
    }

    [[nodiscard]] constexpr Rect Union(const Rect& other) const noexcept
    {
        if (IsEmpty())
        {
            return other;
        }
        if (other.IsEmpty())
        {
            return *this;
        }

        const T min_x = Left()   < other.Left()   ? Left()   : other.Left();
        const T min_y = Top()    < other.Top()    ? Top()    : other.Top();
        const T max_x = Right()  > other.Right()  ? Right()  : other.Right();
        const T max_y = Bottom() > other.Bottom() ? Bottom() : other.Bottom();

        return Rect(min_x, min_y, max_x - min_x, max_y - min_y);
    }

    constexpr void ExpandToInclude(const Vec2<T>& point) noexcept
    {
        if (IsEmpty())
        {
            x = point.x;
            y = point.y;
            width = static_cast<T>(0);
            height = static_cast<T>(0);
            return;
        }

        const T min_x = Left()   < point.x ? Left()   : point.x;
        const T min_y = Top()    < point.y ? Top()    : point.y;
        const T max_x = Right()  > point.x ? Right()  : point.x;
        const T max_y = Bottom() > point.y ? Bottom() : point.y;

        x = min_x;
        y = min_y;
        width = max_x - min_x;
        height = max_y - min_y;
    }

    [[nodiscard]] constexpr Rect Expanded(T amount) const noexcept
    {
        return Rect(x - amount, y - amount, width + amount * static_cast<T>(2),
                    height + amount * static_cast<T>(2));
    }

    [[nodiscard]] constexpr Rect Expanded(T amount_x, T amount_y) const noexcept
    {
        return Rect(x - amount_x, y - amount_y,
                    width + amount_x * static_cast<T>(2),
                    height + amount_y * static_cast<T>(2));
    }

    [[nodiscard]] constexpr Rect Translated(T delta_x, T delta_y) const noexcept
    {
        return Rect(x + delta_x, y + delta_y, width, height);
    }

    [[nodiscard]] constexpr Rect Translated(const Vec2<T>& delta) const noexcept
    {
        return Translated(delta.x, delta.y);
    }

    [[nodiscard]] constexpr Rect Scaled(T scale) const noexcept
    {
        return Rect(x * scale, y * scale, width * scale, height * scale);
    }

    [[nodiscard]] constexpr Rect ScaledAroundCenter(T scale) const noexcept
    {
        const Vec2<T> center = Center();
        const T new_width  = width * scale;
        const T new_height = height * scale;
        return Rect(center.x - new_width  * static_cast<T>(0.5),
                    center.y - new_height * static_cast<T>(0.5),
                    new_width,
                    new_height);
    }

    [[nodiscard]] constexpr Rect ClampInside(const Rect& boundary) const noexcept
    {
        T new_x = x;
        T new_y = y;
        T new_width = width;
        T new_height = height;

        if (new_width > boundary.width)
        {
            new_width = boundary.width;
        }
        if (new_height > boundary.height)
        {
            new_height = boundary.height;
        }

        if (new_x < boundary.x)
        {
            new_x = boundary.x;
        }
        if (new_y < boundary.y)
        {
            new_y = boundary.y;
        }
        if (new_x + new_width > boundary.Right())
        {
            new_x = boundary.Right() - new_width;
        }
        if (new_y + new_height > boundary.Bottom())
        {
            new_y = boundary.Bottom() - new_height;
        }

        return Rect(new_x, new_y, new_width, new_height);
    }

    [[nodiscard]] constexpr bool operator==(const Rect& other) const noexcept
    {
        return x == other.x && y == other.y
            && width == other.width && height == other.height;
    }

    [[nodiscard]] constexpr bool operator!=(const Rect& other) const noexcept
    {
        return !(*this == other);
    }

    [[nodiscard]] bool IsNearlyEqual(const Rect& other,
                                     T epsilon = static_cast<T>(kEpsilonF)) const noexcept
    {
        return ellindyer::core::math::IsNearlyZero<T>(x - other.x, epsilon)
            && ellindyer::core::math::IsNearlyZero<T>(y - other.y, epsilon)
            && ellindyer::core::math::IsNearlyZero<T>(width - other.width, epsilon)
            && ellindyer::core::math::IsNearlyZero<T>(height - other.height, epsilon);
    }

    [[nodiscard]] bool IsFinite() const noexcept
    {
        return ellindyer::core::math::IsFinite(x)
            && ellindyer::core::math::IsFinite(y)
            && ellindyer::core::math::IsFinite(width)
            && ellindyer::core::math::IsFinite(height);
    }

    [[nodiscard]] std::string ToString() const
    {
        return "[" + std::to_string(x) + ", " + std::to_string(y)
             + ", " + std::to_string(width) + ", " + std::to_string(height) + "]";
    }
};

using Rectf = Rect<float>;
using Rectd = Rect<double>;
using Recti = Rect<int>;

} // namespace ellindyer::core::math
