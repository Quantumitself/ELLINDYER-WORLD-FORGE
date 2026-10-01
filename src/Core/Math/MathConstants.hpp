#pragma once

#include <limits>

namespace ellindyer::core::math
{

inline constexpr double kPi        = 3.14159265358979323846;
inline constexpr double kTwoPi     = 6.28318530717958647692;
inline constexpr double kHalfPi    = 1.57079632679489661923;
inline constexpr double kQuarterPi = 0.78539816339744830961;
inline constexpr double kE         = 2.71828182845904523536;
inline constexpr double kSqrt2     = 1.41421356237309504880;

inline constexpr float  kPiF       = static_cast<float>(kPi);
inline constexpr float  kTwoPiF    = static_cast<float>(kTwoPi);
inline constexpr float  kHalfPiF   = static_cast<float>(kHalfPi);
inline constexpr float  kEpsilonF  = std::numeric_limits<float>::epsilon();
inline constexpr double kEpsilonD  = std::numeric_limits<double>::epsilon();

} // namespace ellindyer::core::math
