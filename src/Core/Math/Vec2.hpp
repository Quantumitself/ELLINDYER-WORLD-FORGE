#pragma once

#include <cmath>
#include <cstddef>
#include <string>
#include <type_traits>

#include "Core/Math/MathConstants.hpp"
#include "Core/Math/MathUtils.hpp"

namespace ellindyer::core::math
{

template <typename T>
struct Vec2
{
    static_assert(std::is_arithmetic_v<T>, "Vec2 requires an arithmetic type.");

    T x = static_cast<T>(0);
    T y = static_cast<T>(0);

    constexpr Vec2() noexcept = default;

    constexpr Vec2(T in_x, T in_y) noexcept
        : x(in_x)
        , y(in_y)
    {
    }

    template <typename U>
    constexpr explicit Vec2(const Vec2<U>& other) noexcept
        : x(static_cast<T>(other.x))
        , y(static_cast<T>(other.y))
    {
    }

    [[nodiscard]] static constexpr Vec2 Zero() noexcept
    {
        return Vec2(static_cast<T>(0), static_cast<T>(0));
    }

    [[nodiscard]] static constexpr Vec2 One() noexcept
    {
        return Vec2(static_cast<T>(1), static_cast<T>(1));
    }

    [[nodiscard]] static constexpr Vec2 UnitX() noexcept
    {
        return Vec2(static_cast<T>(1), static_cast<T>(0));
    }

    [[nodiscard]] static constexpr Vec2 UnitY() noexcept
    {
        return Vec2(static_cast<T>(0), static_cast<T>(1));
    }

    [[nodiscard]] static constexpr Vec2 Up() noexcept
    {
        return Vec2(static_cast<T>(0), static_cast<T>(1));
    }

    [[nodiscard]] static constexpr Vec2 Down() noexcept
    {
        return Vec2(static_cast<T>(0), static_cast<T>(-1));
    }

    [[nodiscard]] static constexpr Vec2 Left() noexcept
    {
        return Vec2(static_cast<T>(-1), static_cast<T>(0));
    }

    [[nodiscard]] static constexpr Vec2 Right() noexcept
    {
        return Vec2(static_cast<T>(1), static_cast<T>(0));
    }

    [[nodiscard]] constexpr T operator[](std::size_t index) const noexcept
    {
        return index == 0 ? x : y;
    }

    [[nodiscard]] constexpr T& operator[](std::size_t index) noexcept
    {
        return index == 0 ? x : y;
    }

    [[nodiscard]] constexpr Vec2 operator+() const noexcept
    {
        return *this;
    }

    [[nodiscard]] constexpr Vec2 operator-() const noexcept
    {
        return Vec2(-x, -y);
    }

    [[nodiscard]] constexpr Vec2 operator+(const Vec2& rhs) const noexcept
    {
        return Vec2(x + rhs.x, y + rhs.y);
    }

    [[nodiscard]] constexpr Vec2 operator-(const Vec2& rhs) const noexcept
    {
        return Vec2(x - rhs.x, y - rhs.y);
    }

    [[nodiscard]] constexpr Vec2 operator*(T scalar) const noexcept
    {
        return Vec2(x * scalar, y * scalar);
    }

    [[nodiscard]] constexpr Vec2 operator/(T scalar) const noexcept
    {
        return Vec2(x / scalar, y / scalar);
    }

    [[nodiscard]] constexpr Vec2 operator*(const Vec2& rhs) const noexcept
    {
        return Vec2(x * rhs.x, y * rhs.y);
    }

    [[nodiscard]] constexpr Vec2 operator/(const Vec2& rhs) const noexcept
    {
        return Vec2(x / rhs.x, y / rhs.y);
    }

    constexpr Vec2& operator+=(const Vec2& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr Vec2& operator-=(const Vec2& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr Vec2& operator*=(T scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr Vec2& operator/=(T scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    constexpr Vec2& operator*=(const Vec2& rhs) noexcept
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    constexpr Vec2& operator/=(const Vec2& rhs) noexcept
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    [[nodiscard]] constexpr bool operator==(const Vec2& rhs) const noexcept
    {
        return x == rhs.x && y == rhs.y;
    }

    [[nodiscard]] constexpr bool operator!=(const Vec2& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    [[nodiscard]] constexpr T LengthSquared() const noexcept
    {
        return x * x + y * y;
    }

    [[nodiscard]] T Length() const noexcept
    {
        return Sqrt(LengthSquared());
    }

    [[nodiscard]] T DistanceSquared(const Vec2& other) const noexcept
    {
        const T dx = x - other.x;
        const T dy = y - other.y;
        return dx * dx + dy * dy;
    }

    [[nodiscard]] T Distance(const Vec2& other) const noexcept
    {
        return Sqrt(DistanceSquared(other));
    }

    [[nodiscard]] T Dot(const Vec2& other) const noexcept
    {
        return x * other.x + y * other.y;
    }

    [[nodiscard]] constexpr T Cross(const Vec2& other) const noexcept
    {
        return x * other.y - y * other.x;
    }

    [[nodiscard]] T Angle() const noexcept
    {
        return Atan2(y, x);
    }

    [[nodiscard]] T AngleTo(const Vec2& other) const noexcept
    {
        return Atan2(Cross(other), Dot(other));
    }

    [[nodiscard]] Vec2 Normalized() const noexcept
    {
        const T length = Length();
        if (ellindyer::core::math::IsNearlyZero<T>(length, static_cast<T>(kEpsilonF)))
        {
            return Zero();
        }
        return Vec2(x / length, y / length);
    }

    [[nodiscard]] bool Normalize(T epsilon = static_cast<T>(kEpsilonF)) noexcept
    {
        const T length = Length();
        if (ellindyer::core::math::IsNearlyZero<T>(length, epsilon))
        {
            return false;
        }
        x /= length;
        y /= length;
        return true;
    }

    [[nodiscard]] constexpr Vec2 Perpendicular() const noexcept
    {
        return Vec2(-y, x);
    }

    [[nodiscard]] constexpr Vec2 PerpendicularRight() const noexcept
    {
        return Vec2(y, -x);
    }

    [[nodiscard]] Vec2 LerpTo(const Vec2& target, T t) const noexcept
    {
        return Vec2(
            ellindyer::core::math::Lerp(x, target.x, t),
            ellindyer::core::math::Lerp(y, target.y, t));
    }

    [[nodiscard]] constexpr Vec2 Abs() const noexcept
    {
        return Vec2(
            x < static_cast<T>(0) ? static_cast<T>(-x) : x,
            y < static_cast<T>(0) ? static_cast<T>(-y) : y);
    }

    [[nodiscard]] constexpr Vec2 Min(const Vec2& other) const noexcept
    {
        return Vec2(x < other.x ? x : other.x, y < other.y ? y : other.y);
    }

    [[nodiscard]] constexpr Vec2 Max(const Vec2& other) const noexcept
    {
        return Vec2(x > other.x ? x : other.x, y > other.y ? y : other.y);
    }

    [[nodiscard]] constexpr Vec2 Clamp(const Vec2& minimum, const Vec2& maximum) const noexcept
    {
        return Vec2(
            x < minimum.x ? minimum.x : (x > maximum.x ? maximum.x : x),
            y < minimum.y ? minimum.y : (y > maximum.y ? maximum.y : y));
    }

    [[nodiscard]] bool IsNearlyEqual(const Vec2& other,
                                     T epsilon = static_cast<T>(kEpsilonF)) const noexcept
    {
        return ellindyer::core::math::IsNearlyZero<T>(x - other.x, epsilon)
            && ellindyer::core::math::IsNearlyZero<T>(y - other.y, epsilon);
    }

    [[nodiscard]] bool IsNearlyZero(T epsilon = static_cast<T>(kEpsilonF)) const noexcept
    {
        return ellindyer::core::math::IsNearlyZero<T>(x, epsilon)
            && ellindyer::core::math::IsNearlyZero<T>(y, epsilon);
    }

    [[nodiscard]] bool IsFinite() const noexcept
    {
        return ellindyer::core::math::IsFinite(x) && ellindyer::core::math::IsFinite(y);
    }

    [[nodiscard]] std::string ToString() const
    {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }

    [[nodiscard]] static Vec2 FromAngle(T radians) noexcept
    {
        return Vec2(Cos(radians), Sin(radians));
    }

    [[nodiscard]] static Vec2 FromAngleDegrees(T degrees) noexcept
    {
        return FromAngle(DegToRad(degrees));
    }

    [[nodiscard]] static Vec2 Rotated(const Vec2& vector, T radians) noexcept
    {
        const T cosine = Cos(radians);
        const T sine   = Sin(radians);
        return Vec2(
            vector.x * cosine - vector.y * sine,
            vector.x * sine   + vector.y * cosine);
    }

    [[nodiscard]] Vec2 Rotate(T radians) const noexcept
    {
        return Rotated(*this, radians);
    }

    [[nodiscard]] Vec2 RotatedAround(const Vec2& pivot, T radians) const noexcept
    {
        const Vec2 offset = *this - pivot;
        return pivot + Rotated(offset, radians);
    }

    [[nodiscard]] static Vec2 Reflect(const Vec2& vector, const Vec2& normal) noexcept
    {
        const T dot = vector.Dot(normal);
        return vector - normal * (static_cast<T>(2) * dot);
    }

    [[nodiscard]] static Vec2 Project(const Vec2& vector, const Vec2& onto) noexcept
    {
        const T onto_length_sq = onto.LengthSquared();
        if (ellindyer::core::math::IsNearlyZero<T>(onto_length_sq, static_cast<T>(kEpsilonF)))
        {
            return Zero();
        }
        const T scale = vector.Dot(onto) / onto_length_sq;
        return onto * scale;
    }
};

template <typename T>
[[nodiscard]] constexpr Vec2<T> operator*(T scalar, const Vec2<T>& vector) noexcept
{
    return vector * scalar;
}

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec2i = Vec2<int>;

} // namespace ellindyer::core::math
