#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Core/Settings/SettingsValue.hpp"

namespace ellindyer::core::settings
{

class SettingsStore
{
public:
    SettingsStore();
    explicit SettingsStore(std::filesystem::path file_path);
    ~SettingsStore();

    SettingsStore(const SettingsStore&) = delete;
    SettingsStore& operator=(const SettingsStore&) = delete;
    SettingsStore(SettingsStore&&) noexcept = default;
    SettingsStore& operator=(SettingsStore&&) noexcept = default;

    void SetFilePath(std::filesystem::path file_path);

    [[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept;

    [[nodiscard]] bool HasFilePath() const noexcept;

    void Clear();

    [[nodiscard]] bool Has(std::string_view key) const noexcept;

    [[nodiscard]] std::size_t GetCount() const noexcept;

    [[nodiscard]] std::vector<std::string> GetKeys() const;

    void Remove(std::string_view key);

    // Typed setters

    void SetBoolean(std::string_view key, bool value);
    void SetInteger(std::string_view key, std::int64_t value);
    void SetInteger(std::string_view key, int value);
    void SetFloat(std::string_view key, double value);
    void SetFloat(std::string_view key, float value);
    void SetString(std::string_view key, std::string value);
    void SetString(std::string_view key, std::string_view value);
    void SetValue(std::string_view key, SettingsValue value);

    // Typed getters with defaults

    [[nodiscard]] bool GetBoolean(std::string_view key, bool fallback = false) const noexcept;
    [[nodiscard]] std::int64_t GetInteger(std::string_view key,
                                          std::int64_t fallback = 0) const noexcept;
    [[nodiscard]] double GetFloat(std::string_view key,
                                  double fallback = 0.0) const noexcept;
    [[nodiscard]] std::string GetString(std::string_view key,
                                        std::string fallback = std::string{}) const;
    [[nodiscard]] SettingsValue GetValue(std::string_view key) const;

    [[nodiscard]] const SettingsValue* TryFind(std::string_view key) const noexcept;

    // Persistence

    [[nodiscard]] ellindyer::core::Result<void> Load();

    [[nodiscard]] ellindyer::core::Result<void> LoadFrom(const std::filesystem::path& path);

    [[nodiscard]] ellindyer::core::Result<void> Save() const;

    [[nodiscard]] ellindyer::core::Result<void> SaveTo(const std::filesystem::path& path) const;

private:
    std::filesystem::path                        file_path_;
    std::unordered_map<std::string, SettingsValue> values_;
};

} // namespace ellindyer::core::settings
