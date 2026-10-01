#pragma once

#include <cstdint>
#include <string>

#include <imgui.h>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Menu/MenuBar.hpp"

namespace ellindyer::ui::shell
{

struct ShellWindowConfiguration
{
    std::string title;
    std::string subtitle;
    bool        show_menu_bar   = true;
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

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

    void SetTitle(std::string title);
    void SetSubtitle(std::string subtitle);

    [[nodiscard]] const std::string& GetTitle() const noexcept;
    [[nodiscard]] const std::string& GetSubtitle() const noexcept;

    [[nodiscard]] ellindyer::ui::menu::MenuBar& GetMenuBar() noexcept;
    [[nodiscard]] const ellindyer::ui::menu::MenuBar& GetMenuBar() const noexcept;

    void SetMenuBarVisible(bool visible) noexcept;
    [[nodiscard]] bool IsMenuBarVisible() const noexcept;

    // Height that the shell window occupies at the top of the viewport.
    // The DockSpace host must be placed below this.
    [[nodiscard]] float GetConsumedHeight() const noexcept;

private:
    ShellWindowConfiguration     configuration_{};
    ellindyer::ui::menu::MenuBar menu_bar_{};
    bool                         menu_bar_visible_ = true;
    float                        consumed_height_ = 92.0f;
};

} // namespace ellindyer::ui::shell
