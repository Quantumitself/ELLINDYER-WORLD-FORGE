#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Toolbar/ToolbarItem.hpp"

namespace ellindyer::ui::toolbar
{

struct ToolbarStyle
{
    float   height           = 34.0f;
    float   padding_x        = 10.0f;
    float   padding_y        = 4.0f;
    float   item_spacing     = 4.0f;
    bool    show_separators  = true;
};

class Toolbar
{
public:
    using CommandHandler = std::function<void(const std::string& identifier)>;

    Toolbar();
    ~Toolbar();

    Toolbar(const Toolbar&) = delete;
    Toolbar& operator=(const Toolbar&) = delete;
    Toolbar(Toolbar&&) noexcept = delete;
    Toolbar& operator=(Toolbar&&) noexcept = delete;

    void Clear();

    void SetStyle(const ToolbarStyle& style);

    [[nodiscard]] const ToolbarStyle& GetStyle() const noexcept;

    void AddItem(ToolbarItem item);

    void AddSeparator();

    void AddSpacer(float width);

    void SetCommandHandler(CommandHandler handler);

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

    void SetItemEnabled(const std::string& identifier, bool enabled);

    void SetItemChecked(const std::string& identifier, bool checked);

    void SetItemLabel(const std::string& identifier, std::string label);

    [[nodiscard]] std::size_t GetItemCount() const noexcept;

    [[nodiscard]] float GetHeight() const noexcept;

private:
    void RenderItem(const ToolbarItem& item,
                    const ellindyer::ui::fonts::FontSet& fonts,
                    bool& first_item);

    ToolbarItem* FindItem(const std::string& identifier);

    ToolbarStyle             style_{};
    std::vector<ToolbarItem> items_;
    CommandHandler           command_handler_;
};

} // namespace ellindyer::ui::toolbar
