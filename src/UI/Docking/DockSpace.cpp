#include "UI/Docking/DockSpace.hpp"

#include <algorithm>

#include <imgui_internal.h>

namespace ellindyer::ui::docking
{

namespace
{

constexpr float kSpacing = 0.0f;

} // namespace

DockSpace::DockSpace()
{
    SplitterStyle splitter_style{};
    splitter_style.thickness    = layout_.splitter_thickness;
    splitter_style.hit_padding  = 3.0f;

    left_splitter_.SetStyle(splitter_style);
    left_splitter_.SetOrientation(SplitterOrientation::Vertical);
    left_splitter_.SetIdentifier("##DockSplitterLeft");

    right_splitter_.SetStyle(splitter_style);
    right_splitter_.SetOrientation(SplitterOrientation::Vertical);
    right_splitter_.SetIdentifier("##DockSplitterRight");

    bottom_splitter_.SetStyle(splitter_style);
    bottom_splitter_.SetOrientation(SplitterOrientation::Horizontal);
    bottom_splitter_.SetIdentifier("##DockSplitterBottom");
}

DockSpace::~DockSpace() = default;

void DockSpace::SetLayout(const DockSpaceLayout& layout)
{
    layout_ = layout;

    SplitterStyle splitter_style{};
    splitter_style.thickness   = layout_.splitter_thickness;
    splitter_style.hit_padding = 3.0f;

    left_splitter_.SetStyle(splitter_style);
    right_splitter_.SetStyle(splitter_style);
    bottom_splitter_.SetStyle(splitter_style);
}

const DockSpaceLayout& DockSpace::GetLayout() const noexcept
{
    return layout_;
}

void DockSpace::AddPanel(DockZone zone, std::shared_ptr<DockPanel> panel)
{
    if (!panel)
    {
        return;
    }
    panels_.push_back(PanelEntry{std::move(panel), zone});
}

void DockSpace::ClearPanels()
{
    panels_.clear();
}

void DockSpace::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    const ImVec2 available = ImGui::GetContentRegionAvail();

    float left_width = 0.0f;
    float right_width = 0.0f;
    float center_width = 0.0f;
    float bottom_height = 0.0f;
    float center_height = 0.0f;

    ComputeLayout(available, left_width, right_width, center_width,
                  bottom_height, center_height);

    last_left_width_    = left_width;
    last_right_width_   = right_width;
    last_center_width_  = center_width;
    last_bottom_height_ = bottom_height;
    last_center_height_ = center_height;

    const float splitter_thickness = left_splitter_.GetThickness();

    bool first_rendered = false;

    if (left_visible_ && left_width > 0.0f)
    {
        RenderZonePanels(DockZone::Left, left_width, center_height + bottom_height
                                                       + splitter_thickness
                                                       + (bottom_visible_ ? 0.0f : 0.0f),
                         fonts);
        first_rendered = true;

        if (right_visible_ || center_visible_ || bottom_visible_)
        {
            ImGui::SameLine(0.0f, kSpacing);

            const float splitter_length = center_height + bottom_height
                                          + (bottom_visible_ ? splitter_thickness : 0.0f);

            if (left_splitter_.Render(splitter_length))
            {
                const float delta = ImGui::GetIO().MouseDelta.x;
                if (delta != 0.0f)
                {
                    const float new_left = left_width + delta;
                    const float clamped = std::clamp(new_left,
                                                     layout_.left_min_width,
                                                     available.x - layout_.right_min_width
                                                       - layout_.center_min_width
                                                       - 2.0f * splitter_thickness);
                    layout_.left_fraction = clamped / available.x;
                }
            }

            ImGui::SameLine(0.0f, kSpacing);
        }
    }

    if (center_visible_ || (bottom_visible_ && center_width > 0.0f))
    {
        if (first_rendered && !ImGui::GetCurrentWindow()->DC.CursorPosPrevLine
                                  .y)
        {
            // no-op, retained for clarity of layout flow
        }

        ImGui::BeginGroup();

        if (center_visible_ && center_height > 0.0f)
        {
            RenderZonePanels(DockZone::Center, center_width, center_height, fonts);
        }

        if (bottom_visible_ && bottom_height > 0.0f)
        {
            if (center_visible_ && center_height > 0.0f)
            {
                const float bottom_splitter_length = center_width;
                if (bottom_splitter_.Render(bottom_splitter_length))
                {
                    const float delta = ImGui::GetIO().MouseDelta.y;
                    if (delta != 0.0f)
                    {
                        const float new_bottom = bottom_height - delta;
                        const float clamped = std::clamp(new_bottom,
                                                         layout_.bottom_min_height,
                                                         available.y - layout_.center_min_height
                                                           - splitter_thickness);
                        layout_.bottom_fraction = clamped / available.y;
                    }
                }
            }
            RenderZonePanels(DockZone::Bottom, center_width, bottom_height, fonts);
        }

        ImGui::EndGroup();
        first_rendered = true;

        if (right_visible_ && right_width > 0.0f)
        {
            ImGui::SameLine(0.0f, kSpacing);

            const float splitter_length = center_height + bottom_height
                                          + (bottom_visible_ ? splitter_thickness : 0.0f);

            if (right_splitter_.Render(splitter_length))
            {
                const float delta = ImGui::GetIO().MouseDelta.x;
                if (delta != 0.0f)
                {
                    const float new_right = right_width - delta;
                    const float clamped = std::clamp(new_right,
                                                     layout_.right_min_width,
                                                     available.x - layout_.left_min_width
                                                       - layout_.center_min_width
                                                       - 2.0f * splitter_thickness);
                    layout_.right_fraction = clamped / available.x;
                }
            }
        }
    }

    if (right_visible_ && right_width > 0.0f)
    {
        ImGui::SameLine(0.0f, kSpacing);
        RenderZonePanels(DockZone::Right, right_width, center_height + bottom_height
                                                         + (bottom_visible_ ? splitter_thickness : 0.0f),
                         fonts);
    }
}

void DockSpace::SetZoneVisible(DockZone zone, bool visible) noexcept
{
    switch (zone)
    {
    case DockZone::Left:   left_visible_   = visible; break;
    case DockZone::Right:  right_visible_  = visible; break;
    case DockZone::Bottom: bottom_visible_ = visible; break;
    case DockZone::Center: center_visible_ = visible; break;
    }
}

bool DockSpace::IsZoneVisible(DockZone zone) const noexcept
{
    switch (zone)
    {
    case DockZone::Left:   return left_visible_;
    case DockZone::Right:  return right_visible_;
    case DockZone::Bottom: return bottom_visible_;
    case DockZone::Center: return center_visible_;
    }
    return false;
}

void DockSpace::ResetSplitters() noexcept
{
    layout_.left_fraction   = 0.20f;
    layout_.right_fraction  = 0.22f;
    layout_.bottom_fraction = 0.24f;
}

float DockSpace::GetLeftWidth() const noexcept
{
    return last_left_width_;
}

float DockSpace::GetRightWidth() const noexcept
{
    return last_right_width_;
}

float DockSpace::GetCenterWidth() const noexcept
{
    return last_center_width_;
}

float DockSpace::GetBottomHeight() const noexcept
{
    return last_bottom_height_;
}

float DockSpace::GetCenterHeight() const noexcept
{
    return last_center_height_;
}

void DockSpace::ComputeLayout(const ImVec2& available,
                              float& left_width,
                              float& right_width,
                              float& center_width,
                              float& bottom_height,
                              float& center_height) const
{
    const float splitter_thickness = left_splitter_.GetThickness();

    const int visible_side_zones =
        (left_visible_ ? 1 : 0) + (right_visible_ ? 1 : 0);
    const int visible_horizontal_zones =
        (bottom_visible_ ? 1 : 0);

    const float side_splitter_space =
        static_cast<float>(visible_side_zones) * splitter_thickness;

    const float usable_width = available.x - side_splitter_space;

    float left = 0.0f;
    float right = 0.0f;

    if (left_visible_)
    {
        left = available.x * layout_.left_fraction;
        left = std::max(left, layout_.left_min_width);
    }

    if (right_visible_)
    {
        right = available.x * layout_.right_fraction;
        right = std::max(right, layout_.right_min_width);
    }

    float center = usable_width - left - right;

    if (center < layout_.center_min_width)
    {
        const float deficit = layout_.center_min_width - center;
        const float shrink_left  = std::max(0.0f, left  - layout_.left_min_width);
        const float shrink_right = std::max(0.0f, right - layout_.right_min_width);
        const float shrink_total = shrink_left + shrink_right;

        if (shrink_total > 0.0f)
        {
            left  = std::max(layout_.left_min_width,  left  - deficit * (shrink_left  / shrink_total));
            right = std::max(layout_.right_min_width, right - deficit * (shrink_right / shrink_total));
        }

        center = usable_width - left - right;
        if (center < 0.0f)
        {
            center = 0.0f;
        }
    }

    const float bottom_splitter_space =
        static_cast<float>(visible_horizontal_zones) * splitter_thickness;

    const float usable_height = available.y - bottom_splitter_space;

    float bottom = 0.0f;
    if (bottom_visible_)
    {
        bottom = available.y * layout_.bottom_fraction;
        bottom = std::max(bottom, layout_.bottom_min_height);
    }

    float center_height_local = usable_height - bottom;

    if (center_height_local < layout_.center_min_height)
    {
        const float deficit = layout_.center_min_height - center_height_local;
        const float shrinkable_bottom = std::max(0.0f, bottom - layout_.bottom_min_height);

        if (shrinkable_bottom > 0.0f)
        {
            const float shrink = std::min(deficit, shrinkable_bottom);
            bottom -= shrink;
        }

        center_height_local = usable_height - bottom;
        if (center_height_local < 0.0f)
        {
            center_height_local = 0.0f;
        }
    }

    left_width    = left;
    right_width   = right;
    center_width  = center;
    bottom_height = bottom;
    center_height = center_height_local;
}

void DockSpace::RenderZonePanels(DockZone zone,
                                 float width,
                                 float height,
                                 const ellindyer::ui::fonts::FontSet& fonts)
{
    (void)fonts;

    bool first_in_zone = true;

    for (PanelEntry& entry : panels_)
    {
        if (entry.zone != zone)
        {
            continue;
        }
        if (!entry.panel || !entry.panel->IsVisible())
        {
            continue;
        }

        if (!first_in_zone)
        {
            ImGui::SameLine(0.0f, kSpacing);
        }

        entry.panel->Render(width, height);
        first_in_zone = false;
    }
}

} // namespace ellindyer::ui::docking
