#include "UI/Menu/MenuItem.hpp"

#include <utility>

namespace ellindyer::ui::menu
{

MenuItem MakeCommand(std::string label,
                     std::string identifier,
                     std::function<void()> action,
                     std::string shortcut)
{
    MenuItem item{};
    item.kind       = MenuItemKind::Command;
    item.label      = std::move(label);
    item.identifier = std::move(identifier);
    item.action     = std::move(action);
    item.shortcut   = std::move(shortcut);
    return item;
}

MenuItem MakeSubmenu(std::string label, std::vector<MenuItem> children)
{
    MenuItem item{};
    item.kind     = MenuItemKind::Submenu;
    item.label    = std::move(label);
    item.children = std::move(children);
    return item;
}

MenuItem MakeSeparator()
{
    MenuItem item{};
    item.kind = MenuItemKind::Separator;
    return item;
}

void SetEnabled(MenuItem& item, bool enabled) noexcept
{
    item.enabled = enabled;
}

void SetChecked(MenuItem& item, bool checked) noexcept
{
    item.has_check = true;
    item.checked   = checked;
}

bool HasShortcut(const MenuItem& item) noexcept
{
    return !item.shortcut.empty();
}

} // namespace ellindyer::ui::menu
