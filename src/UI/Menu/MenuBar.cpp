#include "UI/Menu/MenuBar.hpp"

#include <imgui.h>

#include <utility>

namespace ellindyer::ui::menu
{

MenuBar::MenuBar() = default;

MenuBar::~MenuBar() = default;

void MenuBar::Clear()
{
    menus_.clear();
}

void MenuBar::AddMenu(std::string title, std::vector<MenuItem> items)
{
    if (title.empty())
    {
        return;
    }

    MenuEntry entry{};
    entry.title = std::move(title);
    entry.items = std::move(items);
    menus_.push_back(std::move(entry));
}

bool MenuBar::HasMenu(const std::string& title) const noexcept
{
    for (const MenuEntry& entry : menus_)
    {
        if (entry.title == title)
        {
            return true;
        }
    }
    return false;
}

std::size_t MenuBar::GetMenuCount() const noexcept
{
    return menus_.size();
}

void MenuBar::SetCommandHandler(CommandHandler handler)
{
    command_handler_ = std::move(handler);
}

void MenuBar::Render()
{
    if (menus_.empty())
    {
        return;
    }

    if (!ImGui::BeginMenuBar())
    {
        return;
    }

    for (MenuEntry& entry : menus_)
    {
        if (!entry.enabled)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::BeginMenu(entry.title.c_str()))
        {
            RenderMenuItems(entry.title, entry.items);
            ImGui::EndMenu();
        }

        if (!entry.enabled)
        {
            ImGui::EndDisabled();
        }
    }

    ImGui::EndMenuBar();
}

void MenuBar::SetMenuEnabled(const std::string& title, bool enabled)
{
    for (MenuEntry& entry : menus_)
    {
        if (entry.title == title)
        {
            entry.enabled = enabled;
            return;
        }
    }
}

void MenuBar::SetItemEnabled(const std::string& menu_title,
                             const std::string& item_identifier,
                             bool enabled)
{
    for (MenuEntry& entry : menus_)
    {
        if (entry.title != menu_title)
        {
            continue;
        }

        MenuItem* found = nullptr;
        if (FindItem(entry.items, item_identifier, found) && found != nullptr)
        {
            found->enabled = enabled;
        }
        return;
    }
}

void MenuBar::SetItemChecked(const std::string& menu_title,
                             const std::string& item_identifier,
                             bool checked)
{
    for (MenuEntry& entry : menus_)
    {
        if (entry.title != menu_title)
        {
            continue;
        }

        MenuItem* found = nullptr;
        if (FindItem(entry.items, item_identifier, found) && found != nullptr)
        {
            found->has_check = true;
            found->checked   = checked;
        }
        return;
    }
}

void MenuBar::RenderMenuItems(const std::string& /*menu_title*/,
                              const std::vector<MenuItem>& items)
{
    for (const MenuItem& item : items)
    {
        switch (item.kind)
        {
        case MenuItemKind::Separator:
            ImGui::Separator();
            break;

        case MenuItemKind::Command:
        {
            if (!item.enabled)
            {
                ImGui::BeginDisabled();
            }

            const bool selected = item.has_check ? item.checked : false;
            const char* shortcut = item.shortcut.empty() ? nullptr : item.shortcut.c_str();

            if (ImGui::MenuItem(item.label.c_str(), shortcut, selected))
            {
                Dispatch(item.identifier);
            }

            if (!item.enabled)
            {
                ImGui::EndDisabled();
            }
            break;
        }

        case MenuItemKind::Submenu:
        {
            if (!item.enabled)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::BeginMenu(item.label.c_str()))
            {
                RenderMenuItems(item.label, item.children);
                ImGui::EndMenu();
            }

            if (!item.enabled)
            {
                ImGui::EndDisabled();
            }
            break;
        }
        }
    }
}

void MenuBar::Dispatch(const std::string& identifier)
{
    for (MenuEntry& entry : menus_)
    {
        MenuItem* found = nullptr;
        if (FindItem(entry.items, identifier, found) && found != nullptr)
        {
            if (found->action)
            {
                found->action();
            }
            if (command_handler_ && !identifier.empty())
            {
                command_handler_(identifier);
            }
            return;
        }
    }
}

bool MenuBar::FindItem(std::vector<MenuItem>& items,
                       const std::string& identifier,
                       MenuItem*& out_item)
{
    for (MenuItem& item : items)
    {
        if (!item.identifier.empty() && item.identifier == identifier)
        {
            out_item = &item;
            return true;
        }

        if (item.kind == MenuItemKind::Submenu && !item.children.empty())
        {
            if (FindItem(item.children, identifier, out_item))
            {
                return true;
            }
        }
    }
    return false;
}

} // namespace ellindyer::ui::menu
