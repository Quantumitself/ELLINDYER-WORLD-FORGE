#pragma once

#include <type_traits>

#include "Core/Math/MathConstants.hpp"
#include "Core/Math/MathUtils.hpp"
#include "Core/Math/Vec2.hpp"

namespace ellindyer::core::math
{

template <typename T>
struct Transform2D
{
    static_assert(std::is_floating_point_v<T>, "Transform2D requires a floating point type.");

    Vec2<T> position = Vec2<T>::Zero();
    Vec2<T> scale    = Vec2<T>::One();
    T       rotation = static_cast<T>(0);

    constexpr Transform2D() noexcept = default;

    constexpr Transform2D(const Vec2<T>& in_position,
                          const Vec2<T>& in_scale,
                          T in_rotation) noexcept
        : position(in_position)
        , scale(in_scale)
        , rotation(in_rotation)
    {
    }

    [[nodiscard]] static constexpr Transform2D Identity() noexcept
    {
        return Transform2D(Vec2<T>::Zero(), Vec2<T>::One(), static_cast<T>(0));
    }

    [[nodiscard]] static Transform2D FromTranslation(const Vec2<T>& translation) noexcept
    {
        return Transform2D(translation, Vec2<T>::One(), static_cast<T>(0));
    }

    [[nodiscard]] static Transform2D FromScale(const Vec2<T>& uniform_scale) noexcept
    {
        return Transform2D(Vec2<T>::Zero(), uniform_scale, static_cast<T>(0));
    }

    [[nodiscard]] static Transform2D FromRotation(T radians) noexcept
    {
        return Transform2D(Vec2<T>::Zero(), Vec2<T>::One(), radians);
    }

    [[nodiscard]] Vec2<T> Apply(const Vec2<T>& point) const noexcept
    {
        const Vec2<T> scaled(point.x * scale.x, point.y * scale.y);
        const Vec2<T> rotated = Vec2<T>::Rotated(scaled, rotation);
        return rotated + position;
    }

    [[nodiscard]] Vec2<T> ApplyVector(const Vec2<T>& vector) const noexcept
    {
        const Vec2<T> scaled(vector.x * scale.x, vector.y * scale.y);
        return Vec2<T>::Rotated(scaled, rotation);
    }

    [[nodiscard]] Vec2<T> InverseApply(const Vec2<T>& point) const noexcept
    {
        const Vec2<T> offset = point - position;
        const Vec2<T> unrotated = Vec2<T>::Rotated(offset, -rotation);
        return Vec2<T>(
            scale.x != static_cast<T>(0) ? unrotated.x / scale.x : static_cast<T>(0),
            scale.y != static_cast<T>(0) ? unrotated.y / scale.y : static_cast<T>(0));
    }

    [[nodiscard]] Transform2D Inverse() const noexcept
    {
        Transform2D result{};
        result.rotation = -rotation;

        const T cosine = Cos(rotation);
        const T sine   = Sin(rotation);
        const Vec2<T> negated = -position;
        const Vec2<T> rotated = Vec2<T>(
            negated.x * cosine - negated.y * sine,
            negated.x * sine   + negated.y * cosine);

        result.position = Vec2<T>(
            scale.x != static_cast<T>(0) ? rotated.x / scale.x : static_cast<T>(0),
            scale.y != static_cast<T>(0) ? rotated.y / scale.y : static_cast<T>(0));

        result.scale = Vec2<T>(
            scale.x != static_cast<T>(0) ? static_cast<T>(1) / scale.x : static_cast<T>(0),
            scale.y != static_cast<T>(0) ? static_cast<T>(1) / scale.y : static_cast<T>(0));

        return result;
    }

    [[nodiscard]] Transform2D Composed(const Transform2D& child) const noexcept
    {
        Transform2D result{};
        result.scale    = Vec2<T>(scale.x * child.scale.x, scale.y * child.scale.y);
        result.rotation = rotation + child.rotation;
        result.position = Apply(child.position);
        return result;
    }

    [[nodiscard]] static Transform2D Compose(const Transform2D& parent,
                                             const Transform2D& child) noexcept
    {
        return parent.Composed(child);
    }
};

using Transform2Df = Transform2D<float>;
using Transform2Dd = Transform2D<double>;

} // namespace ellindyer::core::math
