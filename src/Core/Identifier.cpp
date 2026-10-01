#include "Core/Identifier.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <random>
#include <utility>

namespace ellindyer::core
{

namespace
{

constexpr char kHexDigits[] = "0123456789abcdef";

constexpr std::size_t kHexPrefixLength = 2;   // "0x"
constexpr std::size_t kHexValueLength  = 16;  // 16 hex digits for 64-bit value

[[nodiscard]] bool IsHexDigit(char c) noexcept
{
    return (c >= '0' && c <= '9')
        || (c >= 'a' && c <= 'f')
        || (c >= 'A' && c <= 'F');
}

[[nodiscard]] int HexValue(char c) noexcept
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return 10 + (c - 'a');
    }
    if (c >= 'A' && c <= 'F')
    {
        return 10 + (c - 'A');
    }
    return -1;
}

[[nodiscard]] std::uint64_t MixBits(std::uint64_t value) noexcept
{
    value ^= value >> 33U;
    value *= 0xff51afd7ed558ccdULL;
    value ^= value >> 33U;
    value *= 0xc4ceb9fe1a85ec53ULL;
    value ^= value >> 33U;
    return value;
}

[[nodiscard]] std::uint64_t HashString(std::string_view text) noexcept
{
    std::uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
    for (char c : text)
    {
        hash ^= static_cast<std::uint64_t>(static_cast<unsigned char>(c));
        hash *= 1099511628211ULL; // FNV-1a prime
    }
    return hash;
}

[[nodiscard]] std::uint64_t GenerateRandomSeed() noexcept
{
    std::random_device device;
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    std::uint64_t seed = static_cast<std::uint64_t>(device());
    seed ^= static_cast<std::uint64_t>(now);
    seed ^= reinterpret_cast<std::uintptr_t>(&device);

    static std::atomic<std::uint64_t> counter{0};
    seed ^= counter.fetch_add(1, std::memory_order_relaxed) * 0x9e3779b97f4a7c15ULL;

    return MixBits(seed);
}

} // namespace

Identifier::Identifier(ValueType value) noexcept
    : value_(value)
{
}

Identifier::Identifier(std::string_view text)
    : value_(HashString(text))
{
    if (value_ == 0)
    {
        value_ = 1;
    }
}

Identifier Identifier::Generate()
{
    std::uint64_t value = GenerateRandomSeed();
    if (value == 0)
    {
        value = 1;
    }
    return Identifier(value);
}

Identifier Identifier::Nil() noexcept
{
    return Identifier(static_cast<ValueType>(0));
}

bool Identifier::TryParse(std::string_view text, Identifier& out_identifier) noexcept
{
    if (text.size() != (kHexPrefixLength + kHexValueLength))
    {
        return false;
    }

    if (text[0] != '0' || (text[1] != 'x' && text[1] != 'X'))
    {
        return false;
    }

    std::uint64_t value = 0;
    for (std::size_t i = kHexPrefixLength; i < text.size(); ++i)
    {
        const int digit = HexValue(text[i]);
        if (digit < 0)
        {
            return false;
        }
        value = (value << 4U) | static_cast<std::uint64_t>(digit);
    }

    out_identifier = Identifier(value);
    return true;
}

Identifier::ValueType Identifier::Value() const noexcept
{
    return value_;
}

bool Identifier::IsNil() const noexcept
{
    return value_ == 0;
}

std::string Identifier::ToString() const
{
    std::string result;
    result.reserve(kHexPrefixLength + kHexValueLength);
    result.push_back('0');
    result.push_back('x');

    for (int shift = (static_cast<int>(kHexValueLength) - 1) * 4; shift >= 0; shift -= 4)
    {
        const std::uint64_t nibble = (value_ >> static_cast<unsigned>(shift)) & 0xFU;
        result.push_back(kHexDigits[nibble]);
    }

    return result;
}

std::string Identifier::ToShortString() const
{
    std::string result;
    result.reserve(kHexPrefixLength + kHexValueLength);
    result.push_back('0');
    result.push_back('x');

    bool started = false;
    for (int shift = (static_cast<int>(kHexValueLength) - 1) * 4; shift >= 0; shift -= 4)
    {
        const std::uint64_t nibble = (value_ >> static_cast<unsigned>(shift)) & 0xFU;
        if (!started && nibble == 0 && shift != 0)
        {
            continue;
        }
        started = true;
        result.push_back(kHexDigits[nibble]);
    }

    return result;
}

bool Identifier::operator==(const Identifier& other) const noexcept
{
    return value_ == other.value_;
}

bool Identifier::operator!=(const Identifier& other) const noexcept
{
    return value_ != other.value_;
}

bool Identifier::operator<(const Identifier& other) const noexcept
{
    return value_ < other.value_;
}

bool Identifier::operator>(const Identifier& other) const noexcept
{
    return value_ > other.value_;
}

bool Identifier::operator<=(const Identifier& other) const noexcept
{
    return value_ <= other.value_;
}

bool Identifier::operator>=(const Identifier& other) const noexcept
{
    return value_ >= other.value_;
}

std::size_t IdentifierHash::operator()(const Identifier& identifier) const noexcept
{
    return static_cast<std::size_t>(MixBits(identifier.Value()));
}

bool IdentifierLess::operator()(const Identifier& lhs, const Identifier& rhs) const noexcept
{
    return lhs.Value() < rhs.Value();
}

bool IdentifierEqual::operator()(const Identifier& lhs, const Identifier& rhs) const noexcept
{
    return lhs.Value() == rhs.Value();
}

Identifier MakeIdentifier(std::string_view text)
{
    return Identifier(text);
}

} // namespace ellindyer::core
