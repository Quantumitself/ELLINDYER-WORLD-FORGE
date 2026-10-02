#include "Core/Settings/SettingsStore.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <system_error>
#include <utility>

#include "Core/ErrorCode.hpp"
#include "Core/FileHelpers.hpp"

namespace ellindyer::core::settings
{

namespace
{

constexpr const char* kSectionHeader = "[settings]";
constexpr const char* kKeyValueSep   = " = ";
constexpr const char* kBooleanTrue   = "true";
constexpr const char* kBooleanFalse  = "false";
constexpr const char* kTypeBool      = "b:";
constexpr const char* kTypeInt       = "i:";
constexpr const char* kTypeFloat     = "f:";
constexpr const char* kTypeString    = "s:";

std::string EscapeString(std::string_view input)
{
    std::string result;
    result.reserve(input.size());
    for (char c : input)
    {
        switch (c)
        {
        case '\\': result += "\\\\"; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\t': result += "\\t"; break;
        case '"':  result += "\\\""; break;
        default:   result.push_back(c); break;
        }
    }
    return result;
}

std::string UnescapeString(std::string_view input)
{
    std::string result;
    result.reserve(input.size());
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        const char c = input[i];
        if (c == '\\' && i + 1 < input.size())
        {
            const char next = input[i + 1];
            switch (next)
            {
            case '\\': result.push_back('\\'); ++i; break;
            case 'n':  result.push_back('\n'); ++i; break;
            case 'r':  result.push_back('\r'); ++i; break;
            case 't':  result.push_back('\t'); ++i; break;
            case '"':  result.push_back('"');  ++i; break;
            default:   result.push_back(c);    break;
            }
        }
        else
        {
            result.push_back(c);
        }
    }
    return result;
}

std::string TrimWhitespace(std::string_view text)
{
    std::size_t start = 0;
    std::size_t end = text.size();
    while (start < end && (text[start] == ' ' || text[start] == '\t'
        || text[start] == '\r' || text[start] == '\n'))
    {
        ++start;
    }
    while (end > start && (text[end - 1] == ' ' || text[end - 1] == '\t'
        || text[end - 1] == '\r' || text[end - 1] == '\n'))
    {
        --end;
    }
    return std::string(text.substr(start, end - start));
}

std::string EncodeValue(const SettingsValue& value)
{
    switch (value.GetKind())
    {
    case SettingsValueKind::Boolean:
        return std::string(kTypeBool) + (value.AsBoolean() ? kBooleanTrue : kBooleanFalse);

    case SettingsValueKind::Integer:
        return std::string(kTypeInt) + std::to_string(value.AsInteger());

    case SettingsValueKind::Float:
    {
        std::ostringstream stream;
        stream.precision(17);
        stream << value.AsFloat();
        return std::string(kTypeFloat) + stream.str();
    }

    case SettingsValueKind::String:
        return std::string(kTypeString) + "\"" + EscapeString(value.AsString()) + "\"";

    case SettingsValueKind::None:
        break;
    }

    return std::string{};
}

SettingsValue DecodeValue(const std::string& encoded)
{
    if (encoded.empty())
    {
        return SettingsValue{};
    }

    const std::string trimmed = TrimWhitespace(encoded);

    if (trimmed.rfind(kTypeBool, 0) == 0)
    {
        const std::string body = trimmed.substr(2);
        return SettingsValue{body == kBooleanTrue};
    }

    if (trimmed.rfind(kTypeInt, 0) == 0)
    {
        const std::string body = trimmed.substr(2);
        try
        {
            return SettingsValue{static_cast<std::int64_t>(std::stoll(body))};
        }
        catch (...)
        {
            return SettingsValue{};
        }
    }

    if (trimmed.rfind(kTypeFloat, 0) == 0)
    {
        const std::string body = trimmed.substr(2);
        try
        {
            return SettingsValue{std::stod(body)};
        }
        catch (...)
        {
            return SettingsValue{};
        }
    }

    if (trimmed.rfind(kTypeString, 0) == 0)
    {
        std::string body = trimmed.substr(2);
        if (!body.empty() && body.front() == '"')
        {
            body.erase(0, 1);
        }
        if (!body.empty() && body.back() == '"')
        {
            body.pop_back();
        }
        return SettingsValue{UnescapeString(body)};
    }

    return SettingsValue{};
}

} // namespace

SettingsStore::SettingsStore() = default;

SettingsStore::SettingsStore(std::filesystem::path file_path)
    : file_path_(std::move(file_path))
{
}

SettingsStore::~SettingsStore() = default;

void SettingsStore::SetFilePath(std::filesystem::path file_path)
{
    file_path_ = std::move(file_path);
}

const std::filesystem::path& SettingsStore::GetFilePath() const noexcept
{
    return file_path_;
}

bool SettingsStore::HasFilePath() const noexcept
{
    return !file_path_.empty();
}

void SettingsStore::Clear()
{
    values_.clear();
}

bool SettingsStore::Has(std::string_view key) const noexcept
{
    return values_.find(std::string(key)) != values_.end();
}

std::size_t SettingsStore::GetCount() const noexcept
{
    return values_.size();
}

std::vector<std::string> SettingsStore::GetKeys() const
{
    std::vector<std::string> keys;
    keys.reserve(values_.size());
    for (const auto& entry : values_)
    {
        keys.push_back(entry.first);
    }
    std::sort(keys.begin(), keys.end());
    return keys;
}

void SettingsStore::Remove(std::string_view key)
{
    values_.erase(std::string(key));
}

void SettingsStore::SetBoolean(std::string_view key, bool value)
{
    SetValue(key, SettingsValue{value});
}

void SettingsStore::SetInteger(std::string_view key, std::int64_t value)
{
    SetValue(key, SettingsValue{value});
}

void SettingsStore::SetInteger(std::string_view key, int value)
{
    SetValue(key, SettingsValue{static_cast<std::int64_t>(value)});
}

void SettingsStore::SetFloat(std::string_view key, double value)
{
    SetValue(key, SettingsValue{value});
}

void SettingsStore::SetFloat(std::string_view key, float value)
{
    SetValue(key, SettingsValue{static_cast<double>(value)});
}

void SettingsStore::SetString(std::string_view key, std::string value)
{
    SetValue(key, SettingsValue{std::move(value)});
}

void SettingsStore::SetString(std::string_view key, std::string_view value)
{
    SetValue(key, SettingsValue{std::string(value)});
}

void SettingsStore::SetValue(std::string_view key, SettingsValue value)
{
    if (key.empty())
    {
        return;
    }
    values_[std::string(key)] = std::move(value);
}

bool SettingsStore::GetBoolean(std::string_view key, bool fallback) const noexcept
{
    const SettingsValue* value = TryFind(key);
    return value != nullptr ? value->AsBoolean(fallback) : fallback;
}

std::int64_t SettingsStore::GetInteger(std::string_view key,
                                       std::int64_t fallback) const noexcept
{
    const SettingsValue* value = TryFind(key);
    return value != nullptr ? value->AsInteger(fallback) : fallback;
}

double SettingsStore::GetFloat(std::string_view key,
                               double fallback) const noexcept
{
    const SettingsValue* value = TryFind(key);
    return value != nullptr ? value->AsFloat(fallback) : fallback;
}

std::string SettingsStore::GetString(std::string_view key,
                                     std::string fallback) const
{
    const SettingsValue* value = TryFind(key);
    if (value == nullptr || !value->IsString())
    {
        return fallback;
    }
    return value->AsString();
}

SettingsValue SettingsStore::GetValue(std::string_view key) const
{
    const SettingsValue* value = TryFind(key);
    return value != nullptr ? *value : SettingsValue{};
}

const SettingsValue* SettingsStore::TryFind(std::string_view key) const noexcept
{
    const auto it = values_.find(std::string(key));
    if (it == values_.end())
    {
        return nullptr;
    }
    return &it->second;
}

ellindyer::core::Result<void> SettingsStore::Load()
{
    if (file_path_.empty())
    {
        return ellindyer::core::Result<void>(
            ellindyer::core::MakeError(ellindyer::core::ErrorCode::InvalidState,
                                       "SettingsStore has no file path."));
    }
    return LoadFrom(file_path_);
}

ellindyer::core::Result<void> SettingsStore::LoadFrom(const std::filesystem::path& path)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (path.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Settings file path is empty."));
    }

    std::error_code exists_ec;
    if (!std::filesystem::exists(path, exists_ec))
    {
        return Result<void>{};
    }

    Result<std::string> content = ellindyer::core::FileHelpers::ReadAllText(path);
    if (content.HasError())
    {
        return Result<void>(content.GetError());
    }

    values_.clear();

    std::istringstream stream(content.Value());
    std::string line;

    while (std::getline(stream, line))
    {
        const std::string trimmed = TrimWhitespace(line);
        if (trimmed.empty())
        {
            continue;
        }
        if (trimmed.front() == '#')
        {
            continue;
        }
        if (trimmed == kSectionHeader)
        {
            continue;
        }

        const std::size_t separator = trimmed.find('=');
        if (separator == std::string::npos)
        {
            continue;
        }

        const std::string key = TrimWhitespace(trimmed.substr(0, separator));
        const std::string value_text = TrimWhitespace(trimmed.substr(separator + 1));

        if (key.empty())
        {
            continue;
        }

        SettingsValue value = DecodeValue(value_text);
        if (!value.IsNone())
        {
            values_[key] = std::move(value);
        }
    }

    return Result<void>{};
}

ellindyer::core::Result<void> SettingsStore::Save() const
{
    if (file_path_.empty())
    {
        return ellindyer::core::Result<void>(
            ellindyer::core::MakeError(ellindyer::core::ErrorCode::InvalidState,
                                       "SettingsStore has no file path."));
    }
    return SaveTo(file_path_);
}

ellindyer::core::Result<void> SettingsStore::SaveTo(const std::filesystem::path& path) const
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (path.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Settings file path is empty."));
    }

    std::ostringstream stream;
    stream << "# Ellindyer World Forge settings\n";
    stream << kSectionHeader << '\n';
    stream << '\n';

    std::vector<std::string> keys = GetKeys();
    for (const std::string& key : keys)
    {
        const auto it = values_.find(key);
        if (it == values_.end())
        {
            continue;
        }
        stream << key << kKeyValueSep << EncodeValue(it->second) << '\n';
    }

    return ellindyer::core::FileHelpers::WriteTextAtomically(path, stream.str());
}

} // namespace ellindyer::core::settings
