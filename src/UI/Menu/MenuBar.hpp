#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "UI/Menu/MenuItem.hpp"

namespace ellindyer::ui::menu
{

class MenuBar
{
public:
    using CommandHandler = std::function<void(const std::string& identifier)>;

    MenuBar();
    ~MenuBar();

    MenuBar(const MenuBar&) = delete;
    MenuBar& operator=(const MenuBar&) = delete;
    MenuBar(MenuBar&&) noexcept = delete;
    MenuBar& operator=(MenuBar&&) noexcept = delete;

    void Clear();

    void AddMenu(std::string title, std::vector<MenuItem> items);

    [[nodiscard]] bool HasMenu(const std::string& title) const noexcept;

    [[nodiscard]] std::size_t GetMenuCount() const noexcept;

    void SetCommandHandler(CommandHandler handler);

    void Render();

    void SetMenuEnabled(const std::string& title, bool enabled);

    void SetItemEnabled(const std::string& menu_title,
                        const std::string& item_identifier,
                        bool enabled);

    void SetItemChecked(const std::string& menu_title,
                        const std::string& item_identifier,
                        bool checked);

private:
    struct MenuEntry
    {
        std::string            title;
        std::vector<MenuItem>  items;
        bool                   enabled = true;
    };

    void RenderMenuItems(const std::string& menu_title,
                         const std::vector<MenuItem>& items);

    void Dispatch(const std::string& identifier);

    static bool FindItem(std::vector<MenuItem>& items,
                         const std::string& identifier,
                         MenuItem*& out_item);

    std::vector<MenuEntry> menus_;
    CommandHandler         command_handler_;
};

} // namespace ellindyer::ui::menu
