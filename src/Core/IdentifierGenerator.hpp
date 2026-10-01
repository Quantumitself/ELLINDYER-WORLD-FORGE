#pragma once

#include <cstdint>

#include "Core/Identifier.hpp"

namespace ellindyer::core
{

class IdentifierGenerator
{
public:
    IdentifierGenerator() = delete;

    [[nodiscard]] static Identifier NewIdentifier();

    [[nodiscard]] static Identifier DeterministicIdentifier(std::uint64_t seed);

    [[nodiscard]] static bool IsReserved(std::uint64_t value) noexcept;

private:
    static constexpr std::uint64_t kReservedNil = 0;
};

} // namespace ellindyer::core
