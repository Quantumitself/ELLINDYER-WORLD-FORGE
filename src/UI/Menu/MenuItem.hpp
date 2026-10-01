#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace ellindyer::ui::menu
{

enum class MenuItemKind : std::uint8_t
{
    Command,
    Submenu,
    Separator
};

struct MenuItem
{
    MenuItemKind                        kind = MenuItemKind::Command;
    std::string                         label;
    std::string                         shortcut;
    std::string                         identifier;
    bool                                enabled  = true;
    bool                                checked  = false;
    bool                                has_check = false;
    std::function<void()>               action;
    std::vector<MenuItem>               children;
};

[[nodiscard]] MenuItem MakeCommand(std::string label,
                                   std::string identifier,
                                   std::function<void()> action,
                                   std::string shortcut = std::string{});

[[nodiscard]] MenuItem MakeSubmenu(std::string label,
                                   std::vector<MenuItem> children);

[[nodiscard]] MenuItem MakeSeparator();

void SetEnabled(MenuItem& item, bool enabled) noexcept;

void SetChecked(MenuItem& item, bool checked) noexcept;

[[nodiscard]] bool HasShortcut(const MenuItem& item) noexcept;

} // namespace ellindyer::ui::menu
