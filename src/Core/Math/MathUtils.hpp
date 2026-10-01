#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "Core/Math/MathConstants.hpp"

namespace ellindyer::core::math
{

template <typename T>
[[nodiscard]] constexpr T Clamp(T value, T minimum, T maximum) noexcept
{
    static_assert(std::is_arithmetic_v<T>, "Clamp requires an arithmetic type.");
    return value < minimum ? minimum : (value > maximum ? maximum : value);
}

template <typename T>
[[nodiscard]] constexpr T Clamp01(T value) noexcept
{
    return Clamp<T>(value, static_cast<T>(0), static_cast<T>(1));
}

template <typename T>
[[nodiscard]] constexpr T Min(T a, T b) noexcept
{
    return a < b ? a : b;
}

template <typename T>
[[nodiscard]] constexpr T Max(T a, T b) noexcept
{
    return a > b ? a : b;
}

template <typename T>
[[nodiscard]] constexpr T Abs(T value) noexcept
{
    static_assert(std::is_arithmetic_v<T>, "Abs requires an arithmetic type.");
    return value < static_cast<T>(0) ? static_cast<T>(-value) : value;
}

template <typename T>
[[nodiscard]] constexpr T Square(T value) noexcept
{
    return value * value;
}

template <typename T>
[[nodiscard]] constexpr T Sign(T value) noexcept
{
    return value > static_cast<T>(0) ? static_cast<T>(1)
         : (value < static_cast<T>(0) ? static_cast<T>(-1) : static_cast<T>(0));
}

template <typename T>
[[nodiscard]] inline T Sqrt(T value) noexcept
{
    using std::sqrt;
    return sqrt(value);
}

template <typename T>
[[nodiscard]] inline T Pow(T base, T exponent) noexcept
{
    using std::pow;
    return pow(base, exponent);
}

template <typename T>
[[nodiscard]] inline T Floor(T value) noexcept
{
    using std::floor;
    return floor(value);
}

template <typename T>
[[nodiscard]] inline T Ceil(T value) noexcept
{
    using std::ceil;
    return ceil(value);
}

template <typename T>
[[nodiscard]] inline T Round(T value) noexcept
{
    using std::round;
    return round(value);
}

template <typename T>
[[nodiscard]] inline T Trunc(T value) noexcept
{
    using std::trunc;
    return trunc(value);
}

template <typename T>
[[nodiscard]] inline T Fmod(T value, T divisor) noexcept
{
    using std::fmod;
    return fmod(value, divisor);
}

template <typename T>
[[nodiscard]] inline T Sin(T value) noexcept
{
    using std::sin;
    return sin(value);
}

template <typename T>
[[nodiscard]] inline T Cos(T value) noexcept
{
    using std::cos;
    return cos(value);
}

template <typename T>
[[nodiscard]] inline T Tan(T value) noexcept
{
    using std::tan;
    return tan(value);
}

template <typename T>
[[nodiscard]] inline T Asin(T value) noexcept
{
    using std::asin;
    return asin(value);
}

template <typename T>
[[nodiscard]] inline T Acos(T value) noexcept
{
    using std::acos;
    return acos(value);
}

template <typename T>
[[nodiscard]] inline T Atan(T value) noexcept
{
    using std::atan;
    return atan(value);
}

template <typename T>
[[nodiscard]] inline T Atan2(T y, T x) noexcept
{
    using std::atan2;
    return atan2(y, x);
}

template <typename T>
[[nodiscard]] inline T Exp(T value) noexcept
{
    using std::exp;
    return exp(value);
}

template <typename T>
[[nodiscard]] inline T Log(T value) noexcept
{
    using std::log;
    return log(value);
}

template <typename T>
[[nodiscard]] inline T Log10(T value) noexcept
{
    using std::log10;
    return log10(value);
}

template <typename T>
[[nodiscard]] inline T DegToRad(T degrees) noexcept
{
    return degrees * static_cast<T>(kPi) / static_cast<T>(180);
}

template <typename T>
[[nodiscard]] inline T RadToDeg(T radians) noexcept
{
    return radians * static_cast<T>(180) / static_cast<T>(kPi);
}

template <typename T>
[[nodiscard]] constexpr T Lerp(T a, T b, T t) noexcept
{
    return a + (b - a) * t;
}

template <typename T>
[[nodiscard]] constexpr T InverseLerp(T a, T b, T value) noexcept
{
    return a != b ? (value - a) / (b - a) : static_cast<T>(0);
}

template <typename T>
[[nodiscard]] constexpr T Remap(T in_min, T in_max, T out_min, T out_max, T value) noexcept
{
    const T t = InverseLerp(in_min, in_max, value);
    return Lerp(out_min, out_max, t);
}

template <typename T>
[[nodiscard]] constexpr T SmoothStep(T edge0, T edge1, T value) noexcept
{
    const T t = Clamp<T>((value - edge0) / (edge1 - edge0), static_cast<T>(0), static_cast<T>(1));
    return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

template <typename T>
[[nodiscard]] constexpr bool ApproximatelyEqual(T a, T b, T epsilon) noexcept
{
    return Abs(a - b) <= epsilon;
}

[[nodiscard]] inline bool ApproximatelyEqualF(float a, float b, float epsilon = kEpsilonF * 8.0f) noexcept
{
    return ApproximatelyEqual<float>(a, b, epsilon);
}

[[nodiscard]] inline bool ApproximatelyEqualD(double a, double b, double epsilon = kEpsilonD * 8.0) noexcept
{
    return ApproximatelyEqual<double>(a, b, epsilon);
}

template <typename T>
[[nodiscard]] constexpr bool IsNearlyZero(T value, T epsilon) noexcept
{
    return Abs(value) <= epsilon;
}

[[nodiscard]] inline bool IsNearlyZeroF(float value, float epsilon = kEpsilonF * 8.0f) noexcept
{
    return IsNearlyZero<float>(value, epsilon);
}

[[nodiscard]] inline bool IsNearlyZeroD(double value, double epsilon = kEpsilonD * 8.0) noexcept
{
    return IsNearlyZero<double>(value, epsilon);
}

template <typename T>
[[nodiscard]] constexpr T Wrap(T value, T minimum, T maximum) noexcept
{
    const T range = maximum - minimum;
    if (range <= static_cast<T>(0))
    {
        return minimum;
    }
    T result = Fmod(value - minimum, range);
    if (result < static_cast<T>(0))
    {
        result += range;
    }
    return result + minimum;
}

template <typename T>
[[nodiscard]] constexpr T Repeat(T value, T length) noexcept
{
    return Wrap(value, static_cast<T>(0), length);
}

template <typename T>
[[nodiscard]] constexpr T PingPong(T value, T length) noexcept
{
    const T wrapped = Repeat(value, length * static_cast<T>(2));
    return wrapped <= length ? wrapped : (length * static_cast<T>(2) - wrapped);
}

template <typename T>
[[nodiscard]] inline T MoveTowards(T current, T target, T max_delta) noexcept
{
    const T diff = target - current;
    if (Abs(diff) <= max_delta)
    {
        return target;
    }
    return current + Sign(diff) * max_delta;
}

template <typename T>
[[nodiscard]] constexpr T MinOf(std::initializer_list<T> values) noexcept
{
    T result = *values.begin();
    for (const T& value : values)
    {
        if (value < result)
        {
            result = value;
        }
    }
    return result;
}

template <typename T>
[[nodiscard]] constexpr T MaxOf(std::initializer_list<T> values) noexcept
{
    T result = *values.begin();
    for (const T& value : values)
    {
        if (value > result)
        {
            result = value;
        }
    }
    return result;
}

template <typename T>
[[nodiscard]] constexpr bool IsFinite(T value) noexcept
{
    static_assert(std::is_floating_point_v<T>, "IsFinite requires a floating point type.");
    return std::isfinite(value);
}

template <typename T>
[[nodiscard]] constexpr bool IsNaN(T value) noexcept
{
    static_assert(std::is_floating_point_v<T>, "IsNaN requires a floating point type.");
    return std::isnan(value);
}

} // namespace ellindyer::core::math
