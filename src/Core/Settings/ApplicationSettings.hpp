#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Core/Settings/SettingsStore.hpp"

namespace ellindyer::core::settings
{

struct ApplicationSettings
{
    // Window
    std::uint32_t window_width  = 1400U;
    std::uint32_t window_height = 900U;
    bool          window_maximized = false;
    int           window_x = -1;
    int           window_y = -1;

    // Layout
    float left_panel_fraction   = 0.20f;
    float right_panel_fraction  = 0.22f;
    float bottom_panel_fraction = 0.24f;
    bool  show_project_explorer = true;
    bool  show_workspace        = true;
    bool  show_inspector        = true;
    bool  show_status_bar       = true;
    bool  show_toolbar          = true;
    bool  show_menu_bar         = true;

    // UI
    float ui_scale              = 1.00f;
    float font_size_default     = 15.0f;

    // Logging
    bool logging_enabled        = true;
    bool logging_console        = true;
    bool logging_file           = true;
    std::string logging_level   = "info";

    // Splash
    bool  splash_enabled        = true;
    float splash_minimum_seconds = 1.75f;
    float splash_maximum_seconds = 4.00f;

    // Recent projects
    std::string last_project_path;
};

class ApplicationSettingsSerializer
{
public:
    ApplicationSettingsSerializer() = delete;

    static void Write(const ApplicationSettings& settings, SettingsStore& store);

    static void Read(SettingsStore& store, ApplicationSettings& settings);

    static ApplicationSettings Defaults();
};

class ApplicationSettingsManager
{
public:
    ApplicationSettingsManager();
    ~ApplicationSettingsManager();

    ApplicationSettingsManager(const ApplicationSettingsManager&) = delete;
    ApplicationSettingsManager& operator=(const ApplicationSettingsManager&) = delete;
    ApplicationSettingsManager(ApplicationSettingsManager&&) noexcept = delete;
    ApplicationSettingsManager& operator=(ApplicationSettingsManager&&) noexcept = delete;

    void SetFilePath(std::filesystem::path file_path);

    [[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept;

    [[nodiscard]] ellindyer::core::Result<void> Load();

    [[nodiscard]] ellindyer::core::Result<void> Save();

    [[nodiscard]] ApplicationSettings& GetSettings() noexcept;

    [[nodiscard]] const ApplicationSettings& GetSettings() const noexcept;

    void ResetToDefaults();

private:
    SettingsStore        store_;
    ApplicationSettings  settings_{};
};

} // namespace ellindyer::core::settings
