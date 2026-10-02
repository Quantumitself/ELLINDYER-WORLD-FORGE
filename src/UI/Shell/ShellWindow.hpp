#pragma once

#include <cstdint>
#include <string>

#include <imgui.h>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Menu/MenuBar.hpp"
#include "UI/StatusBar/StatusBar.hpp"
#include "UI/Toolbar/Toolbar.hpp"

namespace ellindyer::ui::shell
{

struct ShellWindowConfiguration
{
    std::string title;
    std::string subtitle;
    bool        show_menu_bar   = true;
    bool        show_toolbar    = true;
    bool        show_status_bar = true;
    bool        show_dockspace  = false;
};

class ShellWindow
{
public:
    ShellWindow();
    ~ShellWindow();

    ShellWindow(const ShellWindow&) = delete;
    ShellWindow& operator=(const ShellWindow&) = delete;
    ShellWindow(ShellWindow&&) noexcept = delete;
    ShellWindow& operator=(ShellWindow&&) noexcept = delete;

    void Configure(const ShellWindowConfiguration& configuration);

    // Renders only the fixed top strip: menu + toolbar + header.
    // The DockSpace host is placed below this strip by ShellLayout.
    void RenderTop(const ellindyer::ui::fonts::FontSet& fonts);

    // Renders only the fixed bottom strip: status bar.
    // Must be called after ShellLayout has rendered the dockable body.
    void RenderBottom(const ellindyer::ui::fonts::FontSet& fonts);

    void SetTitle(std::string title);
    void SetSubtitle(std::string subtitle);

    [[nodiscard]] const std::string& GetTitle() const noexcept;
    [[nodiscard]] const std::string& GetSubtitle() const noexcept;

    [[nodiscard]] ellindyer::ui::menu::MenuBar& GetMenuBar() noexcept;
    [[nodiscard]] const ellindyer::ui::menu::MenuBar& GetMenuBar() const noexcept;

    [[nodiscard]] ellindyer::ui::toolbar::Toolbar& GetToolbar() noexcept;
    [[nodiscard]] const ellindyer::ui::toolbar::Toolbar& GetToolbar() const noexcept;

    [[nodiscard]] ellindyer::ui::statusbar::StatusBar& GetStatusBar() noexcept;
    [[nodiscard]] const ellindyer::ui::statusbar::StatusBar& GetStatusBar() const noexcept;

    void SetMenuBarVisible(bool visible) noexcept;
    [[nodiscard]] bool IsMenuBarVisible() const noexcept;

    void SetToolbarVisible(bool visible) noexcept;
    [[nodiscard]] bool IsToolbarVisible() const noexcept;

    void SetStatusBarVisible(bool visible) noexcept;
    [[nodiscard]] bool IsStatusBarVisible() const noexcept;

    // Height of the fixed top strip (menu + toolbar + header).
    [[nodiscard]] float GetTopConsumedHeight() const noexcept;

    // Height of the fixed bottom strip (status bar).
    [[nodiscard]] float GetBottomConsumedHeight() const noexcept;

private:
    ShellWindowConfiguration         configuration_{};
    ellindyer::ui::menu::MenuBar     menu_bar_{};
    ellindyer::ui::toolbar::Toolbar  toolbar_{};
    ellindyer::ui::statusbar::StatusBar status_bar_{};
    bool                             menu_bar_visible_   = true;
    bool                             toolbar_visible_    = true;
    bool                             status_bar_visible_ = true;
    float                            top_consumed_height_    = 92.0f;
    float                            bottom_consumed_height_ = 30.0f;
};

} // namespace ellindyer::ui::shell
