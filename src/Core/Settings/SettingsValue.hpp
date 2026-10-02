#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace ellindyer::core::settings
{

enum class SettingsValueKind : std::uint8_t
{
    None,
    Boolean,
    Integer,
    Float,
    String
};

class SettingsValue
{
public:
    using Storage = std::variant<std::monostate, bool, std::int64_t, double, std::string>;

    SettingsValue() = default;

    explicit SettingsValue(bool value);
    explicit SettingsValue(std::int64_t value);
    explicit SettingsValue(int value);
    explicit SettingsValue(double value);
    explicit SettingsValue(float value);
    explicit SettingsValue(std::string value);
    explicit SettingsValue(const char* value);

    [[nodiscard]] SettingsValueKind GetKind() const noexcept;

    [[nodiscard]] bool IsNone() const noexcept;
    [[nodiscard]] bool IsBoolean() const noexcept;
    [[nodiscard]] bool IsInteger() const noexcept;
    [[nodiscard]] bool IsFloat() const noexcept;
    [[nodiscard]] bool IsString() const noexcept;

    [[nodiscard]] bool AsBoolean(bool fallback = false) const noexcept;
    [[nodiscard]] std::int64_t AsInteger(std::int64_t fallback = 0) const noexcept;
    [[nodiscard]] double AsFloat(double fallback = 0.0) const noexcept;
    [[nodiscard]] const std::string& AsString() const noexcept;

    [[nodiscard]] std::string ToDisplayString() const;

private:
    Storage storage_{};
};

} // namespace ellindyer::core::settings
