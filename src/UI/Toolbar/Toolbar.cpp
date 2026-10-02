#include "UI/Toolbar/Toolbar.hpp"

#include <imgui.h>

#include <utility>

namespace ellindyer::ui::toolbar
{

namespace
{

constexpr ImGuiWindowFlags kToolbarFlags =
    ImGuiWindowFlags_NoScrollbar
    | ImGuiWindowFlags_NoScrollWithMouse
    | ImGuiWindowFlags_NoBackground;

ImVec4 ColorText()      { return ImVec4(0.90f, 0.90f, 0.90f, 1.00f); }
ImVec4 ColorMuted()     { return ImVec4(0.55f, 0.55f, 0.55f, 1.00f); }
ImVec4 ColorSeparator() { return ImVec4(0.30f, 0.30f, 0.30f, 1.00f); }
ImVec4 ColorActive()    { return ImVec4(0.85f, 0.85f, 0.85f, 1.00f); }

} // namespace

Toolbar::Toolbar() = default;

Toolbar::~Toolbar() = default;

void Toolbar::Clear()
{
    items_.clear();
}

void Toolbar::SetStyle(const ToolbarStyle& style)
{
    style_ = style;
}

const ToolbarStyle& Toolbar::GetStyle() const noexcept
{
    return style_;
}

void Toolbar::AddItem(ToolbarItem item)
{
    items_.push_back(std::move(item));
}

void Toolbar::AddSeparator()
{
    items_.push_back(MakeSeparator());
}

void Toolbar::AddSpacer(float width)
{
    items_.push_back(MakeSpacer(width));
}

void Toolbar::SetCommandHandler(CommandHandler handler)
{
    command_handler_ = std::move(handler);
}

void Toolbar::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style_.padding_x, style_.padding_y));

    ImGui::BeginChild("##MainToolbar",
                      ImVec2(0.0f, style_.height),
                      false,
                      kToolbarFlags);

    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 4.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style_.item_spacing, 4.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

    bool first_item = true;

    for (const ToolbarItem& item : items_)
    {
        RenderItem(item, fonts, first_item);
    }

    ImGui::PopStyleVar(4);

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }

    ImGui::EndChild();

    ImGui::PopStyleVar();
}

void Toolbar::SetItemEnabled(const std::string& identifier, bool enabled)
{
    ToolbarItem* item = FindItem(identifier);
    if (item != nullptr)
    {
        item->enabled = enabled;
    }
}

void Toolbar::SetItemChecked(const std::string& identifier, bool checked)
{
    ToolbarItem* item = FindItem(identifier);
    if (item != nullptr)
    {
        item->checked = checked;
    }
}

void Toolbar::SetItemLabel(const std::string& identifier, std::string label)
{
    ToolbarItem* item = FindItem(identifier);
    if (item != nullptr)
    {
        item->label = std::move(label);
    }
}

std::size_t Toolbar::GetItemCount() const noexcept
{
    return items_.size();
}

float Toolbar::GetHeight() const noexcept
{
    return style_.height;
}

void Toolbar::RenderItem(const ToolbarItem& item,
                         const ellindyer::ui::fonts::FontSet& /*fonts*/,
                         bool& first_item)
{
    switch (item.kind)
    {
    case ToolbarItemKind::Separator:
    {
        if (!style_.show_separators)
        {
            return;
        }

        if (!first_item)
        {
            ImGui::SameLine();
        }

        const ImVec2 cursor = ImGui::GetCursorScreenPos();
        const float height = ImGui::GetFrameHeight() - 4.0f;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddLine(ImVec2(cursor.x + 4.0f, cursor.y + 2.0f),
                           ImVec2(cursor.x + 4.0f, cursor.y + height),
                           ImGui::GetColorU32(ColorSeparator()));

        ImGui::Dummy(ImVec2(9.0f, height));

        first_item = false;
        return;
    }

    case ToolbarItemKind::Spacer:
    {
        if (!first_item)
        {
            ImGui::SameLine();
        }
        ImGui::Dummy(ImVec2(item.spacer_width, 1.0f));
        first_item = false;
        return;
    }

    case ToolbarItemKind::Label:
    {
        if (!first_item)
        {
            ImGui::SameLine();
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
        ImGui::TextUnformatted(item.label.c_str());
        ImGui::PopStyleColor();

        first_item = false;
        return;
    }

    case ToolbarItemKind::Button:
    case ToolbarItemKind::Toggle:
    {
        if (!first_item)
        {
            ImGui::SameLine();
        }

        if (!item.enabled)
        {
            ImGui::BeginDisabled();
        }

        const bool is_toggle = (item.kind == ToolbarItemKind::Toggle);

        if (is_toggle && item.checked)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.32f, 0.32f, 0.32f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.42f, 0.42f, 0.42f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_Text, ColorActive());
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ColorText());
        }

        const bool pressed = ImGui::Button(item.label.c_str());

        if (is_toggle && item.checked)
        {
            ImGui::PopStyleColor(4);
        }
        else
        {
            ImGui::PopStyleColor(1);
        }

        if (!item.enabled)
        {
            ImGui::EndDisabled();
        }

        if (pressed && item.enabled)
        {
            if (item.action)
            {
                item.action();
            }
            if (command_handler_ && !item.identifier.empty())
            {
                command_handler_(item.identifier);
            }
        }

        if (!item.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(item.tooltip.c_str());
            ImGui::EndTooltip();
        }

        first_item = false;
        return;
    }
    }
}

ToolbarItem* Toolbar::FindItem(const std::string& identifier)
{
    if (identifier.empty())
    {
        return nullptr;
    }

    for (ToolbarItem& item : items_)
    {
        if (item.identifier == identifier)
        {
            return &item;
        }
    }
    return nullptr;
}

} // namespace ellindyer::ui::toolbar
