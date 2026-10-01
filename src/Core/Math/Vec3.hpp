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
struct Vec3
{
    static_assert(std::is_arithmetic_v<T>, "Vec3 requires an arithmetic type.");

    T x = static_cast<T>(0);
    T y = static_cast<T>(0);
    T z = static_cast<T>(0);

    constexpr Vec3() noexcept = default;

    constexpr Vec3(T in_x, T in_y, T in_z) noexcept
        : x(in_x)
        , y(in_y)
        , z(in_z)
    {
    }

    template <typename U>
    constexpr explicit Vec3(const Vec3<U>& other) noexcept
        : x(static_cast<T>(other.x))
        , y(static_cast<T>(other.y))
        , z(static_cast<T>(other.z))
    {
    }

    [[nodiscard]] static constexpr Vec3 Zero() noexcept
    {
        return Vec3(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
    }

    [[nodiscard]] static constexpr Vec3 One() noexcept
    {
        return Vec3(static_cast<T>(1), static_cast<T>(1), static_cast<T>(1));
    }

    [[nodiscard]] static constexpr Vec3 UnitX() noexcept
    {
        return Vec3(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));
    }

    [[nodiscard]] static constexpr Vec3 UnitY() noexcept
    {
        return Vec3(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));
    }

    [[nodiscard]] static constexpr Vec3 UnitZ() noexcept
    {
        return Vec3(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
    }

    [[nodiscard]] static constexpr Vec3 Up() noexcept
    {
        return Vec3(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
    }

    [[nodiscard]] static constexpr Vec3 Forward() noexcept
    {
        return Vec3(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));
    }

    [[nodiscard]] static constexpr Vec3 Right() noexcept
    {
        return Vec3(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));
    }

    [[nodiscard]] constexpr T operator[](std::size_t index) const noexcept
    {
        return index == 0 ? x : (index == 1 ? y : z);
    }

    [[nodiscard]] constexpr T& operator[](std::size_t index) noexcept
    {
        return index == 0 ? x : (index == 1 ? y : z);
    }

    [[nodiscard]] constexpr Vec3 operator+() const noexcept
    {
        return *this;
    }

    [[nodiscard]] constexpr Vec3 operator-() const noexcept
    {
        return Vec3(-x, -y, -z);
    }

    [[nodiscard]] constexpr Vec3 operator+(const Vec3& rhs) const noexcept
    {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    [[nodiscard]] constexpr Vec3 operator-(const Vec3& rhs) const noexcept
    {
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    [[nodiscard]] constexpr Vec3 operator*(T scalar) const noexcept
    {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    [[nodiscard]] constexpr Vec3 operator/(T scalar) const noexcept
    {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    [[nodiscard]] constexpr Vec3 operator*(const Vec3& rhs) const noexcept
    {
        return Vec3(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    [[nodiscard]] constexpr Vec3 operator/(const Vec3& rhs) const noexcept
    {
        return Vec3(x / rhs.x, y / rhs.y, z / rhs.z);
    }

    constexpr Vec3& operator+=(const Vec3& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    constexpr Vec3& operator-=(const Vec3& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    constexpr Vec3& operator*=(T scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr Vec3& operator/=(T scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    constexpr Vec3& operator*=(const Vec3& rhs) noexcept
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    constexpr Vec3& operator/=(const Vec3& rhs) noexcept
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    [[nodiscard]] constexpr bool operator==(const Vec3& rhs) const noexcept
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    [[nodiscard]] constexpr bool operator!=(const Vec3& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    [[nodiscard]] constexpr T LengthSquared() const noexcept
    {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] T Length() const noexcept
    {
        return Sqrt(LengthSquared());
    }

    [[nodiscard]] T DistanceSquared(const Vec3& other) const noexcept
    {
        const T dx = x - other.x;
        const T dy = y - other.y;
        const T dz = z - other.z;
        return dx * dx + dy * dy + dz * dz;
    }

    [[nodiscard]] T Distance(const Vec3& other) const noexcept
    {
        return Sqrt(DistanceSquared(other));
    }

    [[nodiscard]] constexpr T Dot(const Vec3& other) const noexcept
    {
        return x * other.x + y * other.y + z * other.z;
    }

    [[nodiscard]] constexpr Vec3 Cross(const Vec3& other) const noexcept
    {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x);
    }

    [[nodiscard]] Vec3 Normalized() const noexcept
    {
        const T length = Length();
        if (ellindyer::core::math::IsNearlyZero<T>(length, static_cast<T>(kEpsilonF)))
        {
            return Zero();
        }
        return Vec3(x / length, y / length, z / length);
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
        z /= length;
        return true;
    }

    [[nodiscard]] Vec3 LerpTo(const Vec3& target, T t) const noexcept
    {
        return Vec3(
            ellindyer::core::math::Lerp(x, target.x, t),
            ellindyer::core::math::Lerp(y, target.y, t),
            ellindyer::core::math::Lerp(z, target.z, t));
    }

    [[nodiscard]] constexpr Vec3 Abs() const noexcept
    {
        return Vec3(
            x < static_cast<T>(0) ? static_cast<T>(-x) : x,
            y < static_cast<T>(0) ? static_cast<T>(-y) : y,
            z < static_cast<T>(0) ? static_cast<T>(-z) : z);
    }

    [[nodiscard]] constexpr Vec3 Min(const Vec3& other) const noexcept
    {
        return Vec3(
            x < other.x ? x : other.x,
            y < other.y ? y : other.y,
            z < other.z ? z : other.z);
    }

    [[nodiscard]] constexpr Vec3 Max(const Vec3& other) const noexcept
    {
        return Vec3(
            x > other.x ? x : other.x,
            y > other.y ? y : other.y,
            z > other.z ? z : other.z);
    }

    [[nodiscard]] constexpr Vec3 Clamp(const Vec3& minimum, const Vec3& maximum) const noexcept
    {
        return Vec3(
            x < minimum.x ? minimum.x : (x > maximum.x ? maximum.x : x),
            y < minimum.y ? minimum.y : (y > maximum.y ? maximum.y : y),
            z < minimum.z ? minimum.z : (z > maximum.z ? maximum.z : z));
    }

    [[nodiscard]] bool IsNearlyEqual(const Vec3& other,
                                     T epsilon = static_cast<T>(kEpsilonF)) const noexcept
    {
        return ellindyer::core::math::IsNearlyZero<T>(x - other.x, epsilon)
            && ellindyer::core::math::IsNearlyZero<T>(y - other.y, epsilon)
            && ellindyer::core::math::IsNearlyZero<T>(z - other.z, epsilon);
    }

    [[nodiscard]] bool IsNearlyZero(T epsilon = static_cast<T>(kEpsilonF)) const noexcept
    {
        return ellindyer::core::math::IsNearlyZero<T>(x, epsilon)
            && ellindyer::core::math::IsNearlyZero<T>(y, epsilon)
            && ellindyer::core::math::IsNearlyZero<T>(z, epsilon);
    }

    [[nodiscard]] bool IsFinite() const noexcept
    {
        return ellindyer::core::math::IsFinite(x)
            && ellindyer::core::math::IsFinite(y)
            && ellindyer::core::math::IsFinite(z);
    }

    [[nodiscard]] std::string ToString() const
    {
        return "(" + std::to_string(x)
             + ", " + std::to_string(y)
             + ", " + std::to_string(z) + ")";
    }

    [[nodiscard]] static Vec3 Reflect(const Vec3& vector, const Vec3& normal) noexcept
    {
        const T dot = vector.Dot(normal);
        return vector - normal * (static_cast<T>(2) * dot);
    }

    [[nodiscard]] static Vec3 Project(const Vec3& vector, const Vec3& onto) noexcept
    {
        const T onto_length_sq = onto.LengthSquared();
        if (ellindyer::core::math::IsNearlyZero<T>(onto_length_sq, static_cast<T>(kEpsilonF)))
        {
            return Zero();
        }
        const T scale = vector.Dot(onto) / onto_length_sq;
        return onto * scale;
    }

    [[nodiscard]] static Vec3 CrossProduct(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return lhs.Cross(rhs);
    }
};

template <typename T>
[[nodiscard]] constexpr Vec3<T> operator*(T scalar, const Vec3<T>& vector) noexcept
{
    return vector * scalar;
}

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec3i = Vec3<int>;

} // namespace ellindyer::core::math
