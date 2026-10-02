#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace ellindyer::ui::toolbar
{

enum class ToolbarItemKind : std::uint8_t
{
    Button,
    Toggle,
    Separator,
    Spacer,
    Label
};

struct ToolbarItem
{
    ToolbarItemKind        kind = ToolbarItemKind::Button;
    std::string            label;
    std::string            tooltip;
    std::string            identifier;
    bool                   enabled = true;
    bool                   checked = false;
    std::function<void()>  action;
    float                  spacer_width = 0.0f;
};

[[nodiscard]] ToolbarItem MakeButton(std::string label,
                                     std::string identifier,
                                     std::function<void()> action,
                                     std::string tooltip = std::string{});

[[nodiscard]] ToolbarItem MakeToggle(std::string label,
                                     std::string identifier,
                                     bool checked,
                                     std::function<void()> action,
                                     std::string tooltip = std::string{});

[[nodiscard]] ToolbarItem MakeSeparator();

[[nodiscard]] ToolbarItem MakeSpacer(float width);

[[nodiscard]] ToolbarItem MakeLabel(std::string label);

void SetToolbarItemEnabled(ToolbarItem& item, bool enabled) noexcept;

void SetToolbarItemChecked(ToolbarItem& item, bool checked) noexcept;

void SetToolbarItemLabel(ToolbarItem& item, std::string label);

} // namespace ellindyer::ui::toolbar
