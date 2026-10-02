#include "Core/Settings/ApplicationSettings.hpp"

#include <utility>

namespace ellindyer::core::settings
{

namespace
{

constexpr const char* kWindowWidthKey         = "window.width";
constexpr const char* kWindowHeightKey        = "window.height";
constexpr const char* kWindowMaximizedKey     = "window.maximized";
constexpr const char* kWindowXKey             = "window.x";
constexpr const char* kWindowYKey             = "window.y";

constexpr const char* kLeftPanelFractionKey   = "layout.left_fraction";
constexpr const char* kRightPanelFractionKey  = "layout.right_fraction";
constexpr const char* kBottomPanelFractionKey = "layout.bottom_fraction";
constexpr const char* kShowProjectExplorerKey = "layout.show_project_explorer";
constexpr const char* kShowWorkspaceKey       = "layout.show_workspace";
constexpr const char* kShowInspectorKey       = "layout.show_inspector";
constexpr const char* kShowStatusBarKey       = "layout.show_status_bar";
constexpr const char* kShowToolbarKey         = "layout.show_toolbar";
constexpr const char* kShowMenuBarKey         = "layout.show_menu_bar";

constexpr const char* kUiScaleKey             = "ui.scale";
constexpr const char* kFontSizeDefaultKey     = "ui.font_size_default";

constexpr const char* kLoggingEnabledKey      = "logging.enabled";
constexpr const char* kLoggingConsoleKey      = "logging.console";
constexpr const char* kLoggingFileKey         = "logging.file";
constexpr const char* kLoggingLevelKey        = "logging.level";

constexpr const char* kSplashEnabledKey       = "splash.enabled";
constexpr const char* kSplashMinKey           = "splash.minimum_seconds";
constexpr const char* kSplashMaxKey           = "splash.maximum_seconds";

constexpr const char* kLastProjectPathKey     = "project.last_path";

} // namespace

void ApplicationSettingsSerializer::Write(const ApplicationSettings& settings,
                                           SettingsStore& store)
{
    store.SetInteger(kWindowWidthKey, static_cast<std::int64_t>(settings.window_width));
    store.SetInteger(kWindowHeightKey, static_cast<std::int64_t>(settings.window_height));
    store.SetBoolean(kWindowMaximizedKey, settings.window_maximized);
    store.SetInteger(kWindowXKey, settings.window_x);
    store.SetInteger(kWindowYKey, settings.window_y);

    store.SetFloat(kLeftPanelFractionKey, settings.left_panel_fraction);
    store.SetFloat(kRightPanelFractionKey, settings.right_panel_fraction);
    store.SetFloat(kBottomPanelFractionKey, settings.bottom_panel_fraction);
    store.SetBoolean(kShowProjectExplorerKey, settings.show_project_explorer);
    store.SetBoolean(kShowWorkspaceKey, settings.show_workspace);
    store.SetBoolean(kShowInspectorKey, settings.show_inspector);
    store.SetBoolean(kShowStatusBarKey, settings.show_status_bar);
    store.SetBoolean(kShowToolbarKey, settings.show_toolbar);
    store.SetBoolean(kShowMenuBarKey, settings.show_menu_bar);

    store.SetFloat(kUiScaleKey, settings.ui_scale);
    store.SetFloat(kFontSizeDefaultKey, settings.font_size_default);

    store.SetBoolean(kLoggingEnabledKey, settings.logging_enabled);
    store.SetBoolean(kLoggingConsoleKey, settings.logging_console);
    store.SetBoolean(kLoggingFileKey, settings.logging_file);
    store.SetString(kLoggingLevelKey, settings.logging_level);

    store.SetBoolean(kSplashEnabledKey, settings.splash_enabled);
    store.SetFloat(kSplashMinKey, settings.splash_minimum_seconds);
    store.SetFloat(kSplashMaxKey, settings.splash_maximum_seconds);

    store.SetString(kLastProjectPathKey, settings.last_project_path);
}

void ApplicationSettingsSerializer::Read(SettingsStore& store, ApplicationSettings& settings)
{
    const ApplicationSettings defaults = Defaults();

    settings.window_width = static_cast<std::uint32_t>(
        store.GetInteger(kWindowWidthKey,
                         static_cast<std::int64_t>(defaults.window_width)));
    settings.window_height = static_cast<std::uint32_t>(
        store.GetInteger(kWindowHeightKey,
                         static_cast<std::int64_t>(defaults.window_height)));
    settings.window_maximized = store.GetBoolean(kWindowMaximizedKey,
                                                 defaults.window_maximized);
    settings.window_x = static_cast<int>(store.GetInteger(
        kWindowXKey, static_cast<std::int64_t>(defaults.window_x)));
    settings.window_y = static_cast<int>(store.GetInteger(
        kWindowYKey, static_cast<std::int64_t>(defaults.window_y)));

    settings.left_panel_fraction = static_cast<float>(store.GetFloat(
        kLeftPanelFractionKey, defaults.left_panel_fraction));
    settings.right_panel_fraction = static_cast<float>(store.GetFloat(
        kRightPanelFractionKey, defaults.right_panel_fraction));
    settings.bottom_panel_fraction = static_cast<float>(store.GetFloat(
        kBottomPanelFractionKey, defaults.bottom_panel_fraction));
    settings.show_project_explorer = store.GetBoolean(kShowProjectExplorerKey,
                                                      defaults.show_project_explorer);
    settings.show_workspace = store.GetBoolean(kShowWorkspaceKey,
                                               defaults.show_workspace);
    settings.show_inspector = store.GetBoolean(kShowInspectorKey,
                                               defaults.show_inspector);
    settings.show_status_bar = store.GetBoolean(kShowStatusBarKey,
                                                defaults.show_status_bar);
    settings.show_toolbar = store.GetBoolean(kShowToolbarKey,
                                             defaults.show_toolbar);
    settings.show_menu_bar = store.GetBoolean(kShowMenuBarKey,
                                              defaults.show_menu_bar);

    settings.ui_scale = static_cast<float>(store.GetFloat(kUiScaleKey,
                                                          defaults.ui_scale));
    settings.font_size_default = static_cast<float>(store.GetFloat(
        kFontSizeDefaultKey, defaults.font_size_default));

    settings.logging_enabled = store.GetBoolean(kLoggingEnabledKey,
                                                defaults.logging_enabled);
    settings.logging_console = store.GetBoolean(kLoggingConsoleKey,
                                                defaults.logging_console);
    settings.logging_file = store.GetBoolean(kLoggingFileKey,
                                             defaults.logging_file);
    settings.logging_level = store.GetString(kLoggingLevelKey,
                                             defaults.logging_level);

    settings.splash_enabled = store.GetBoolean(kSplashEnabledKey,
                                               defaults.splash_enabled);
    settings.splash_minimum_seconds = static_cast<float>(store.GetFloat(
        kSplashMinKey, defaults.splash_minimum_seconds));
    settings.splash_maximum_seconds = static_cast<float>(store.GetFloat(
        kSplashMaxKey, defaults.splash_maximum_seconds));

    settings.last_project_path = store.GetString(kLastProjectPathKey,
                                                 defaults.last_project_path);
}

ApplicationSettings ApplicationSettingsSerializer::Defaults()
{
    return ApplicationSettings{};
}

ApplicationSettingsManager::ApplicationSettingsManager()
    : settings_(ApplicationSettingsSerializer::Defaults())
{
}

ApplicationSettingsManager::~ApplicationSettingsManager() = default;

void ApplicationSettingsManager::SetFilePath(std::filesystem::path file_path)
{
    store_.SetFilePath(std::move(file_path));
}

const std::filesystem::path& ApplicationSettingsManager::GetFilePath() const noexcept
{
    return store_.GetFilePath();
}

ellindyer::core::Result<void> ApplicationSettingsManager::Load()
{
    using ellindyer::core::Result;

    settings_ = ApplicationSettingsSerializer::Defaults();

    const Result<void> load_result = store_.Load();
    if (load_result.HasError())
    {
        return load_result;
    }

    ApplicationSettingsSerializer::Read(store_, settings_);
    return Result<void>{};
}

ellindyer::core::Result<void> ApplicationSettingsManager::Save()
{
    ApplicationSettingsSerializer::Write(settings_, store_);
    return store_.Save();
}

ApplicationSettings& ApplicationSettingsManager::GetSettings() noexcept
{
    return settings_;
}

const ApplicationSettings& ApplicationSettingsManager::GetSettings() const noexcept
{
    return settings_;
}

void ApplicationSettingsManager::ResetToDefaults()
{
    settings_ = ApplicationSettingsSerializer::Defaults();
    store_.Clear();
    ApplicationSettingsSerializer::Write(settings_, store_);
}

} // namespace ellindyer::core::settings
