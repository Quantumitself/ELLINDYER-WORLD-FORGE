#pragma once

#include <utility>

#include "Core/Error.hpp"
#include "Core/Result.hpp"

namespace ellindyer::core
{

template <typename T>
[[nodiscard]] bool Succeeded(const Result<T>& result) noexcept
{
    return result.HasValue();
}

template <typename T>
[[nodiscard]] bool Failed(const Result<T>& result) noexcept
{
    return result.HasError();
}

template <typename T, typename Fn>
[[nodiscard]] auto MapResult(Result<T>&& result, Fn&& mapper)
{
    using U = decltype(mapper(std::declval<T>()));
    if (result.HasError())
    {
        return Result<U>(std::move(result.GetError()));
    }
    return Result<U>(mapper(std::move(result).Value()));
}

template <typename T, typename Fn>
[[nodiscard]] Result<T> AndThen(Result<T>&& result, Fn&& fn)
{
    if (result.HasError())
    {
        return std::move(result);
    }
    return fn(std::move(result).Value());
}

template <typename T>
[[nodiscard]] Error ExtractError(const Result<T>& result)
{
    if (result.HasError())
    {
        return result.GetError();
    }
    return Error::Success();
}

} // namespace ellindyer::core
