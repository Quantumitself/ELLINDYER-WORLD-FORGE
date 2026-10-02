#include "UI/Toolbar/ToolbarItem.hpp"

#include <utility>

namespace ellindyer::ui::toolbar
{

ToolbarItem MakeButton(std::string label,
                       std::string identifier,
                       std::function<void()> action,
                       std::string tooltip)
{
    ToolbarItem item{};
    item.kind       = ToolbarItemKind::Button;
    item.label      = std::move(label);
    item.identifier = std::move(identifier);
    item.action     = std::move(action);
    item.tooltip    = std::move(tooltip);
    return item;
}

ToolbarItem MakeToggle(std::string label,
                       std::string identifier,
                       bool checked,
                       std::function<void()> action,
                       std::string tooltip)
{
    ToolbarItem item{};
    item.kind       = ToolbarItemKind::Toggle;
    item.label      = std::move(label);
    item.identifier = std::move(identifier);
    item.checked    = checked;
    item.action     = std::move(action);
    item.tooltip    = std::move(tooltip);
    return item;
}

ToolbarItem MakeSeparator()
{
    ToolbarItem item{};
    item.kind = ToolbarItemKind::Separator;
    return item;
}

ToolbarItem MakeSpacer(float width)
{
    ToolbarItem item{};
    item.kind         = ToolbarItemKind::Spacer;
    item.spacer_width = width > 0.0f ? width : 8.0f;
    return item;
}

ToolbarItem MakeLabel(std::string label)
{
    ToolbarItem item{};
    item.kind  = ToolbarItemKind::Label;
    item.label = std::move(label);
    return item;
}

void SetToolbarItemEnabled(ToolbarItem& item, bool enabled) noexcept
{
    item.enabled = enabled;
}

void SetToolbarItemChecked(ToolbarItem& item, bool checked) noexcept
{
    item.checked = checked;
}

void SetToolbarItemLabel(ToolbarItem& item, std::string label)
{
    item.label = std::move(label);
}

} // namespace ellindyer::ui::toolbar
