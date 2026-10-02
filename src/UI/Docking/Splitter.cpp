#include "UI/Docking/Splitter.hpp"

#include <algorithm>

namespace ellindyer::ui::docking
{

namespace
{

constexpr float kCursorPadFraction = 0.5f;

} // namespace

Splitter::Splitter(SplitterStyle style)
    : style_(style)
{
}

void Splitter::SetStyle(const SplitterStyle& style)
{
    style_ = style;
}

const SplitterStyle& Splitter::GetStyle() const noexcept
{
    return style_;
}

void Splitter::SetOrientation(SplitterOrientation orientation) noexcept
{
    orientation_ = orientation;
}

SplitterOrientation Splitter::GetOrientation() const noexcept
{
    return orientation_;
}

void Splitter::SetIdentifier(const char* identifier)
{
    identifier_ = identifier != nullptr ? identifier : "##Splitter";
}

const char* Splitter::GetIdentifier() const noexcept
{
    return identifier_;
}

void Splitter::SetBounds(float minimum, float maximum) noexcept
{
    if (minimum > maximum)
    {
        std::swap(minimum, maximum);
    }
    minimum_ = minimum;
    maximum_ = maximum;
    value_   = std::clamp(value_, minimum_, maximum_);
}

float Splitter::GetValue() const noexcept
{
    return value_;
}

void Splitter::SetValue(float value) noexcept
{
    value_ = std::clamp(value, minimum_, maximum_);
}

bool Splitter::IsHovered() const noexcept
{
    return hovered_;
}

bool Splitter::IsActive() const noexcept
{
    return active_;
}

bool Splitter::IsDragged() const noexcept
{
    return dragged_;
}

float Splitter::GetThickness() const noexcept
{
    return style_.thickness;
}

bool Splitter::Render(float length)
{
    hovered_ = false;
    active_  = false;
    dragged_ = false;

    const ImVec2 cursor = ImGui::GetCursorScreenPos();
    const ImVec2 size = (orientation_ == SplitterOrientation::Vertical)
        ? ImVec2(style_.thickness, length)
        : ImVec2(length, style_.thickness);

    ImGui::InvisibleButton(identifier_, size);

    hovered_ = ImGui::IsItemHovered();
    active_  = ImGui::IsItemActive();

    if (active_ && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        dragged_ = true;

        if (orientation_ == SplitterOrientation::Vertical)
        {
            const float delta = ImGui::GetIO().MouseDelta.x;
            value_ = std::clamp(value_ + delta, minimum_, maximum_);
        }
        else
        {
            const float delta = ImGui::GetIO().MouseDelta.y;
            value_ = std::clamp(value_ + delta, minimum_, maximum_);
        }
    }

    DrawBar(length, cursor, hovered_ || active_, active_);

    return dragged_;
}

void Splitter::DrawBar(float length,
                       const ImVec2& cursor,
                       bool hovered,
                       bool active)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const ImVec4 color = active
        ? style_.active_color
        : (hovered ? style_.hover_color : style_.idle_color);

    if (orientation_ == SplitterOrientation::Vertical)
    {
        const float bar_left = cursor.x + (style_.thickness - style_.hit_padding) * kCursorPadFraction;
        const float bar_right = bar_left + style_.hit_padding;
        draw_list->AddRectFilled(
            ImVec2(bar_left, cursor.y),
            ImVec2(bar_right, cursor.y + length),
            ImGui::GetColorU32(color));
    }
    else
    {
        const float bar_top = cursor.y + (style_.thickness - style_.hit_padding) * kCursorPadFraction;
        const float bar_bottom = bar_top + style_.hit_padding;
        draw_list->AddRectFilled(
            ImVec2(cursor.x, bar_top),
            ImVec2(cursor.x + length, bar_bottom),
            ImGui::GetColorU32(color));
    }
}

} // namespace ellindyer::ui::docking
