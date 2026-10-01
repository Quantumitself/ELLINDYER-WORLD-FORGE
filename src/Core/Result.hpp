#pragma once

#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include "Core/Error.hpp"

namespace ellindyer::core
{

template <typename T>
class Result
{
public:
    using ValueType = T;
    using ErrorType = Error;

    static_assert(!std::is_reference_v<T>, "Result<T> does not support reference types.");
    static_assert(!std::is_void_v<T>, "Use Result<void> specialization for void results.");

    Result() = delete;

    Result(T value)
        : storage_(std::in_place_index<0>, std::move(value))
    {
    }

    Result(Error error)
        : storage_(std::in_place_index<1>, std::move(error))
    {
    }

    Result(const Result&) = default;
    Result(Result&&) noexcept = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&) noexcept = default;
    ~Result() = default;

    [[nodiscard]] bool HasValue() const noexcept
    {
        return storage_.index() == 0;
    }

    [[nodiscard]] bool HasError() const noexcept
    {
        return storage_.index() == 1;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return HasValue();
    }

    [[nodiscard]] T& Value() &
    {
        return std::get<0>(storage_);
    }

    [[nodiscard]] const T& Value() const&
    {
        return std::get<0>(storage_);
    }

    [[nodiscard]] T&& Value() &&
    {
        return std::get<0>(std::move(storage_));
    }

    [[nodiscard]] Error& GetError() &
    {
        return std::get<1>(storage_);
    }

    [[nodiscard]] const Error& GetError() const&
    {
        return std::get<1>(storage_);
    }

    [[nodiscard]] T ValueOr(T fallback) const
    {
        if (HasValue())
        {
            return std::get<0>(storage_);
        }
        return fallback;
    }

    template <typename U>
    [[nodiscard]] T ValueOr(U&& fallback) const
    {
        if (HasValue())
        {
            return std::get<0>(storage_);
        }
        return static_cast<T>(std::forward<U>(fallback));
    }

private:
    std::variant<T, Error> storage_;
};

template <>
class Result<void>
{
public:
    using ValueType = void;
    using ErrorType = Error;

    Result() noexcept
        : error_(Error::Success())
    {
    }

    Result(Error error)
        : error_(std::move(error))
    {
    }

    Result(const Result&) = default;
    Result(Result&&) noexcept = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&) noexcept = default;
    ~Result() = default;

    [[nodiscard]] bool HasValue() const noexcept
    {
        return error_.IsSuccess();
    }

    [[nodiscard]] bool HasError() const noexcept
    {
        return error_.IsFailure();
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return HasValue();
    }

    [[nodiscard]] Error& GetError() noexcept
    {
        return error_;
    }

    [[nodiscard]] const Error& GetError() const noexcept
    {
        return error_;
    }

private:
    Error error_;
};

template <typename T>
[[nodiscard]] Result<std::decay_t<T>> MakeResult(T&& value)
{
    return Result<std::decay_t<T>>(std::forward<T>(value));
}

[[nodiscard]] inline Result<void> MakeResult()
{
    return Result<void>{};
}

[[nodiscard]] inline Result<void> MakeResult(Error error)
{
    return Result<void>(std::move(error));
}

} // namespace ellindyer::core
