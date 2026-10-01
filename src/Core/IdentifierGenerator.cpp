#include "Core/IdentifierGenerator.hpp"

namespace ellindyer::core
{

namespace
{

[[nodiscard]] std::uint64_t Mix(std::uint64_t value) noexcept
{
    value ^= value >> 33U;
    value *= 0xff51afd7ed558ccdULL;
    value ^= value >> 33U;
    value *= 0xc4ceb9fe1a85ec53ULL;
    value ^= value >> 33U;
    return value;
}

} // namespace

Identifier IdentifierGenerator::NewIdentifier()
{
    Identifier identifier = Identifier::Generate();
    if (identifier.IsNil())
    {
        identifier = Identifier(1);
    }
    return identifier;
}

Identifier IdentifierGenerator::DeterministicIdentifier(std::uint64_t seed)
{
    std::uint64_t value = Mix(seed);
    if (IsReserved(value))
    {
        value = 1;
    }
    return Identifier(value);
}

bool IdentifierGenerator::IsReserved(std::uint64_t value) noexcept
{
    return value == kReservedNil;
}

} // namespace ellindyer::core
