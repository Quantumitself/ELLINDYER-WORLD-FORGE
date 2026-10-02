#include "Core/Settings/SettingsValue.hpp"

#include <sstream>
#include <utility>

namespace ellindyer::core::settings
{

namespace
{

const std::string kEmptyString{};

} // namespace

SettingsValue::SettingsValue(bool value)
    : storage_(value)
{
}

SettingsValue::SettingsValue(std::int64_t value)
    : storage_(value)
{
}

SettingsValue::SettingsValue(int value)
    : storage_(static_cast<std::int64_t>(value))
{
}

SettingsValue::SettingsValue(double value)
    : storage_(value)
{
}

SettingsValue::SettingsValue(float value)
    : storage_(static_cast<double>(value))
{
}

SettingsValue::SettingsValue(std::string value)
    : storage_(std::move(value))
{
}

SettingsValue::SettingsValue(const char* value)
    : storage_(std::string(value != nullptr ? value : ""))
{
}

SettingsValueKind SettingsValue::GetKind() const noexcept
{
    switch (storage_.index())
    {
    case 0: return SettingsValueKind::None;
    case 1: return SettingsValueKind::Boolean;
    case 2: return SettingsValueKind::Integer;
    case 3: return SettingsValueKind::Float;
    case 4: return SettingsValueKind::String;
    default: break;
    }
    return SettingsValueKind::None;
}

bool SettingsValue::IsNone() const noexcept
{
    return storage_.index() == 0;
}

bool SettingsValue::IsBoolean() const noexcept
{
    return storage_.index() == 1;
}

bool SettingsValue::IsInteger() const noexcept
{
    return storage_.index() == 2;
}

bool SettingsValue::IsFloat() const noexcept
{
    return storage_.index() == 3;
}

bool SettingsValue::IsString() const noexcept
{
    return storage_.index() == 4;
}

bool SettingsValue::AsBoolean(bool fallback) const noexcept
{
    if (const bool* value = std::get_if<bool>(&storage_))
    {
        return *value;
    }
    return fallback;
}

std::int64_t SettingsValue::AsInteger(std::int64_t fallback) const noexcept
{
    if (const std::int64_t* value = std::get_if<std::int64_t>(&storage_))
    {
        return *value;
    }
    if (const double* value = std::get_if<double>(&storage_))
    {
        return static_cast<std::int64_t>(*value);
    }
    return fallback;
}

double SettingsValue::AsFloat(double fallback) const noexcept
{
    if (const double* value = std::get_if<double>(&storage_))
    {
        return *value;
    }
    if (const std::int64_t* value = std::get_if<std::int64_t>(&storage_))
    {
        return static_cast<double>(*value);
    }
    return fallback;
}

const std::string& SettingsValue::AsString() const noexcept
{
    if (const std::string* value = std::get_if<std::string>(&storage_))
    {
        return *value;
    }
    return kEmptyString;
}

std::string SettingsValue::ToDisplayString() const
{
    switch (GetKind())
    {
    case SettingsValueKind::None:
        return std::string{"<none>"};

    case SettingsValueKind::Boolean:
        return AsBoolean() ? std::string{"true"} : std::string{"false"};

    case SettingsValueKind::Integer:
        return std::to_string(AsInteger());

    case SettingsValueKind::Float:
    {
        std::ostringstream stream;
        stream << AsFloat();
        return stream.str();
    }

    case SettingsValueKind::String:
        return AsString();
    }

    return std::string{"<unknown>"};
}

} // namespace ellindyer::core::settings
