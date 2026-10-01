#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace ellindyer::core
{

class Identifier
{
public:
    using ValueType = std::uint64_t;

    Identifier() noexcept = default;

    explicit Identifier(ValueType value) noexcept;

    explicit Identifier(std::string_view text);

    [[nodiscard]] static Identifier Generate();

    [[nodiscard]] static Identifier Nil() noexcept;

    [[nodiscard]] static bool TryParse(std::string_view text, Identifier& out_identifier) noexcept;

    [[nodiscard]] ValueType Value() const noexcept;

    [[nodiscard]] bool IsNil() const noexcept;

    [[nodiscard]] std::string ToString() const;

    [[nodiscard]] std::string ToShortString() const;

    [[nodiscard]] bool operator==(const Identifier& other) const noexcept;

    [[nodiscard]] bool operator!=(const Identifier& other) const noexcept;

    [[nodiscard]] bool operator<(const Identifier& other) const noexcept;

    [[nodiscard]] bool operator>(const Identifier& other) const noexcept;

    [[nodiscard]] bool operator<=(const Identifier& other) const noexcept;

    [[nodiscard]] bool operator>=(const Identifier& other) const noexcept;

private:
    ValueType value_ = 0;
};

struct IdentifierHash
{
    [[nodiscard]] std::size_t operator()(const Identifier& identifier) const noexcept;
};

struct IdentifierLess
{
    [[nodiscard]] bool operator()(const Identifier& lhs, const Identifier& rhs) const noexcept;
};

struct IdentifierEqual
{
    [[nodiscard]] bool operator()(const Identifier& lhs, const Identifier& rhs) const noexcept;
};

[[nodiscard]] Identifier MakeIdentifier(std::string_view text);

} // namespace ellindyer::core

namespace std
{

template <>
struct hash<ellindyer::core::Identifier>
{
    [[nodiscard]] std::size_t operator()(const ellindyer::core::Identifier& identifier) const noexcept
    {
        return ellindyer::core::IdentifierHash{}(identifier);
    }
};

} // namespace std
