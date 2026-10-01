#pragma once

#include <type_traits>

#include "Core/Math/MathConstants.hpp"
#include "Core/Math/MathUtils.hpp"
#include "Core/Math/Rect.hpp"
#include "Core/Math/Vec2.hpp"

namespace ellindyer::core::math
{

template <typename T>
[[nodiscard]] T DistancePointToSegment(const Vec2<T>& point,
                                       const Vec2<T>& segment_start,
                                       const Vec2<T>& segment_end) noexcept
{
    const Vec2<T> segment = segment_end - segment_start;
    const T segment_length_sq = segment.LengthSquared();

    if (ellindyer::core::math::IsNearlyZero<T>(segment_length_sq, static_cast<T>(kEpsilonF)))
    {
        return point.Distance(segment_start);
    }

    T t = (point - segment_start).Dot(segment) / segment_length_sq;
    t = Clamp<T>(t, static_cast<T>(0), static_cast<T>(1));

    const Vec2<T> projection = segment_start + segment * t;
    return point.Distance(projection);
}

template <typename T>
[[nodiscard]] T DistancePointToLine(const Vec2<T>& point,
                                    const Vec2<T>& line_start,
                                    const Vec2<T>& line_end) noexcept
{
    const Vec2<T> line = line_end - line_start;
    const T line_length_sq = line.LengthSquared();

    if (ellindyer::core::math::IsNearlyZero<T>(line_length_sq, static_cast<T>(kEpsilonF)))
    {
        return point.Distance(line_start);
    }

    const T cross = line.Cross(point - line_start);
    return Abs(cross) / Sqrt(line_length_sq);
}

template <typename T>
[[nodiscard]] Vec2<T> ClosestPointOnSegment(const Vec2<T>& point,
                                            const Vec2<T>& segment_start,
                                            const Vec2<T>& segment_end) noexcept
{
    const Vec2<T> segment = segment_end - segment_start;
    const T segment_length_sq = segment.LengthSquared();

    if (ellindyer::core::math::IsNearlyZero<T>(segment_length_sq, static_cast<T>(kEpsilonF)))
    {
        return segment_start;
    }

    T t = (point - segment_start).Dot(segment) / segment_length_sq;
    t = Clamp<T>(t, static_cast<T>(0), static_cast<T>(1));

    return segment_start + segment * t;
}

template <typename T>
[[nodiscard]] bool SegmentsIntersect(const Vec2<T>& a_start,
                                     const Vec2<T>& a_end,
                                     const Vec2<T>& b_start,
                                     const Vec2<T>& b_end) noexcept
{
    const Vec2<T> r = a_end - a_start;
    const Vec2<T> s = b_end - b_start;

    const T denominator = r.Cross(s);
    const Vec2<T> delta = b_start - a_start;

    if (ellindyer::core::math::IsNearlyZero<T>(denominator, static_cast<T>(kEpsilonF)))
    {
        return false;
    }

    const T t = delta.Cross(s) / denominator;
    const T u = delta.Cross(r) / denominator;

    return t >= static_cast<T>(0) && t <= static_cast<T>(1)
        && u >= static_cast<T>(0) && u <= static_cast<T>(1);
}

template <typename T>
[[nodiscard]] bool PointInRect(const Vec2<T>& point, const Rect<T>& rect) noexcept
{
    return rect.Contains(point);
}

template <typename T>
[[nodiscard]] bool RectContainsRect(const Rect<T>& outer, const Rect<T>& inner) noexcept
{
    return outer.Contains(inner);
}

template <typename T>
[[nodiscard]] bool RectsOverlap(const Rect<T>& a, const Rect<T>& b) noexcept
{
    return a.Intersects(b);
}

template <typename T>
[[nodiscard]] T CircleArea(T radius) noexcept
{
    return static_cast<T>(kPi) * radius * radius;
}

template <typename T>
[[nodiscard]] T CircleCircumference(T radius) noexcept
{
    return static_cast<T>(2) * static_cast<T>(kPi) * radius;
}

template <typename T>
[[nodiscard]] bool PointInCircle(const Vec2<T>& point,
                                 const Vec2<T>& center,
                                 T radius) noexcept
{
    return point.DistanceSquared(center) <= radius * radius;
}

template <typename T>
[[nodiscard]] bool CirclesOverlap(const Vec2<T>& center_a, T radius_a,
                                  const Vec2<T>& center_b, T radius_b) noexcept
{
    const T combined = radius_a + radius_b;
    return center_a.DistanceSquared(center_b) <= combined * combined;
}

} // namespace ellindyer::core::math
