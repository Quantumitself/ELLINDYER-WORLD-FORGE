#include "UI/Layout/DockLayout.hpp"

#include <algorithm>

namespace ellindyer::ui::layout
{

namespace
{

constexpr const char* kLeftSplitterId   = "DockLayout.LeftSplitter";
constexpr const char* kRightSplitterId  = "DockLayout.RightSplitter";
constexpr const char* kBottomSplitterId = "DockLayout.BottomSplitter";

} // namespace

DockLayout::DockLayout()
{
    left_splitter_.SetIdentifier(kLeftSplitterId);
    left_splitter_.SetAxis(SplitterAxis::Horizontal);

    right_splitter_.SetIdentifier(kRightSplitterId);
    right_splitter_.SetAxis(SplitterAxis::Horizontal);

    bottom_splitter_.SetIdentifier(kBottomSplitterId);
    bottom_splitter_.SetAxis(SplitterAxis::Vertical);
}

DockLayout::~DockLayout() = default;

void DockLayout::SetStyle(const DockLayoutStyle& style)
{
    style_ = style;
}

const DockLayoutStyle& DockLayout::GetStyle() const noexcept
{
    return style_;
}

void DockLayout::SetLeftPanel(Panel* panel, float width, float min_width, bool visible)
{
    left_panel_  = panel;
    left_width_  = width > 0.0f ? width : style_.default_left_min;
    left_min_    = min_width > 0.0f ? min_width : style_.default_left_min;
    left_visible_ = visible && (panel != nullptr);
}

void DockLayout::SetRightPanel(Panel* panel, float width, float min_width, bool visible)
{
    right_panel_   = panel;
    right_width_   = width > 0.0f ? width : style_.default_right_min;
    right_min_     = min_width > 0.0f ? min_width : style_.default_right_min;
    right_visible_ = visible && (panel != nullptr);
}

void DockLayout::SetCenterPanel(Panel* panel, float min_width, bool visible)
{
    center_panel_   = panel;
    center_min_     = min_width > 0.0f ? min_width : style_.default_center_min;
    center_visible_ = visible && (panel != nullptr);
}

void DockLayout::SetBottomPanel(Panel* panel, float height, float min_height, bool visible)
{
    bottom_panel_   = panel;
    bottom_height_  = height > 0.0f ? height : style_.default_bottom_min;
    bottom_min_     = min_height > 0.0f ? min_height : style_.default_bottom_min;
    bottom_visible_ = visible && (panel != nullptr);
}

void DockLayout::SetLeftVisible(bool visible) noexcept
{
    left_visible_ = visible && (left_panel_ != nullptr);
}

void DockLayout::SetRightVisible(bool visible) noexcept
{
    right_visible_ = visible && (right_panel_ != nullptr);
}

void DockLayout::SetCenterVisible(bool visible) noexcept
{
    center_visible_ = visible && (center_panel_ != nullptr);
}

void DockLayout::SetBottomVisible(bool visible) noexcept
{
    bottom_visible_ = visible && (bottom_panel_ != nullptr);
}

bool DockLayout::IsLeftVisible() const noexcept
{
    return left_visible_;
}

bool DockLayout::IsRightVisible() const noexcept
{
    return right_visible_;
}

bool DockLayout::IsCenterVisible() const noexcept
{
    return center_visible_;
}

bool DockLayout::IsBottomVisible() const noexcept
{
    return bottom_visible_;
}

float DockLayout::GetLeftWidth() const noexcept
{
    return left_width_;
}

float DockLayout::GetRightWidth() const noexcept
{
    return right_width_;
}

float DockLayout::GetBottomHeight() const noexcept
{
    return bottom_height_;
}

void DockLayout::ResetLayout() noexcept
{
    left_width_    = 260.0f;
    right_width_   = 320.0f;
    bottom_height_ = 180.0f;
}

void DockLayout::Render(const ellindyer::ui::fonts::FontSet& fonts,
                        PanelRenderFn left_renderer,   void* left_context,
                        PanelRenderFn right_renderer,  void* right_context,
                        PanelRenderFn center_renderer, void* center_context,
                        PanelRenderFn bottom_renderer, void* bottom_context)
{
    const ImVec2 region_size = ImGui::GetContentRegionAvail();
    if (region_size.x <= 0.0f || region_size.y <= 0.0f)
    {
        return;
    }

    const float splitter_thickness = style_.splitter_thickness;

    // Vertical composition: upper area + optional bottom panel + optional vertical splitter.
    float upper_height = region_size.y;
    float bottom_height_local = bottom_visible_ ? bottom_height_ : 0.0f;

    if (bottom_visible_)
    {
        upper_height -= (bottom_height_local + splitter_thickness);
        if (upper_height < style_.default_center_min)
        {
            const float deficit = style_.default_center_min - upper_height;
            const float shrinkable = bottom_height_local - bottom_min_;
            const float shrink = std::min(deficit, std::max(0.0f, shrinkable));
            bottom_height_local -= shrink;
            upper_height += shrink;
        }
        if (upper_height < 0.0f) upper_height = 0.0f;
        bottom_height_ = bottom_height_local;
    }

    // Horizontal composition for the upper area:
    // [Left][Splitter?][Center][Splitter?][Right]
    float available_width = region_size.x;

    const int visible_side_panels =
        (left_visible_ ? 1 : 0) + (right_visible_ ? 1 : 0);

    const float splitters_width =
        static_cast<float>(visible_side_panels) * splitter_thickness;

    const float horizontal_available =
        available_width - splitters_width;

    float resolved_left  = left_visible_  ? left_width_  : 0.0f;
    float resolved_right = right_visible_ ? right_width_ : 0.0f;

    // Enforce minimums and shrink on overflow.
    {
        float center_width = horizontal_available - resolved_left - resolved_right;

        if (center_width < center_min_)
        {
            const float deficit = center_min_ - center_width;
            const float shrinkable_left  = std::max(0.0f, resolved_left  - left_min_);
            const float shrinkable_right = std::max(0.0f, resolved_right - right_min_);
            const float shrinkable_total = shrinkable_left + shrinkable_right;

            if (shrinkable_total > 0.0f)
            {
                resolved_left  = std::max(left_min_,
                                          resolved_left -
                                          deficit * (shrinkable_left / shrinkable_total));
                resolved_right = std::max(right_min_,
                                          resolved_right -
                                          deficit * (shrinkable_right / shrinkable_total));
            }

            center_width = horizontal_available - resolved_left - resolved_right;
            if (center_width < 0.0f)
            {
                center_width = 0.0f;
            }
        }
    }

    float center_width = horizontal_available - resolved_left - resolved_right;
    if (center_width < 0.0f) center_width = 0.0f;

    // Render top row: Left | Center | Right (with splitters interleaved).
    const ImVec2 top_origin = ImGui::GetCursorScreenPos();

    bool first_rendered = false;

    if (left_visible_ && left_panel_ != nullptr && left_renderer != nullptr)
    {
        ImGui::SetCursorScreenPos(top_origin);
        left_panel_->BeginPanel(resolved_left, upper_height);
        left_renderer(left_context, *left_panel_, resolved_left, upper_height, fonts);
        left_panel_->EndPanel();
        first_rendered = true;
    }

    if (left_visible_ && center_visible_ && center_panel_ != nullptr && center_renderer != nullptr)
    {
        const float splitter_center_x = top_origin.x + resolved_left + splitter_thickness * 0.5f;
        const float splitter_center_y = top_origin.y;

        left_splitter_.SetMinimumPrevious(left_min_);
        left_splitter_.SetMinimumNext(center_min_);

        const float new_left = left_splitter_.Render(resolved_left,
                                                     horizontal_available,
                                                     splitter_center_x,
                                                     splitter_center_y);

        if (new_left != resolved_left)
        {
            resolved_left = new_left;
            left_width_   = new_left;
            center_width  = horizontal_available - resolved_left - resolved_right;
            if (center_width < 0.0f) center_width = 0.0f;

            // Re-render left with updated width.
            ImGui::SetCursorScreenPos(top_origin);
            left_panel_->BeginPanel(resolved_left, upper_height);
            left_renderer(left_context, *left_panel_, resolved_left, upper_height, fonts);
            left_panel_->EndPanel();
        }
    }

    if (center_visible_ && center_panel_ != nullptr && center_renderer != nullptr)
    {
        const float center_x = top_origin.x + (left_visible_ ? resolved_left + splitter_thickness : 0.0f);
        ImGui::SetCursorScreenPos(ImVec2(center_x, top_origin.y));
        center_panel_->BeginPanel(center_width, upper_height);
        center_renderer(center_context, *center_panel_, center_width, upper_height, fonts);
        center_panel_->EndPanel();
        first_rendered = first_rendered || true;
    }

    if (right_visible_ && center_visible_ && right_panel_ != nullptr && right_renderer != nullptr)
    {
        const float splitter_center_x = top_origin.x + resolved_left + splitter_thickness
                                      + center_width + splitter_thickness * 0.5f;
        const float splitter_center_y = top_origin.y;

        right_splitter_.SetMinimumPrevious(center_min_);
        right_splitter_.SetMinimumNext(right_min_);

        const float new_right = right_splitter_.Render(resolved_right,
                                                       horizontal_available,
                                                       splitter_center_x,
                                                       splitter_center_y);

        if (new_right != resolved_right)
        {
            resolved_right = new_right;
            right_width_   = new_right;
            center_width   = horizontal_available - resolved_left - resolved_right;
            if (center_width < 0.0f) center_width = 0.0f;

            // Re-render center with updated width.
            if (center_visible_ && center_panel_ != nullptr && center_renderer != nullptr)
            {
                const float center_x = top_origin.x +
                    (left_visible_ ? resolved_left + splitter_thickness : 0.0f);
                ImGui::SetCursorScreenPos(ImVec2(center_x, top_origin.y));
                center_panel_->BeginPanel(center_width, upper_height);
                center_renderer(center_context, *center_panel_, center_width, upper_height, fonts);
                center_panel_->EndPanel();
            }
        }
    }

    if (right_visible_ && right_panel_ != nullptr && right_renderer != nullptr)
    {
        const float right_x = top_origin.x + resolved_left + splitter_thickness
                            + center_width + splitter_thickness;
        ImGui::SetCursorScreenPos(ImVec2(right_x, top_origin.y));
        right_panel_->BeginPanel(resolved_right, upper_height);
        right_renderer(right_context, *right_panel_, resolved_right, upper_height, fonts);
        right_panel_->EndPanel();
    }

    // Render bottom row when visible.
    if (bottom_visible_ && bottom_panel_ != nullptr && bottom_renderer != nullptr)
    {
        const float bottom_y = top_origin.y + upper_height;

        const float splitter_center_x = top_origin.x + region_size.x * 0.5f;
        const float splitter_center_y = bottom_y + splitter_thickness * 0.5f;

        bottom_splitter_.SetMinimumPrevious(center_min_);
        bottom_splitter_.SetMinimumNext(bottom_min_);

        const float total_vertical = region_size.y;
        const float new_bottom = bottom_splitter_.Render(bottom_height_,
                                                        total_vertical,
                                                        splitter_center_y,
                                                        splitter_center_x);

        if (new_bottom != bottom_height_)
        {
            bottom_height_ = new_bottom;
            upper_height = region_size.y - bottom_height_ - splitter_thickness;
            if (upper_height < 0.0f) upper_height = 0.0f;

            // Re-render upper area panels with updated upper height.
            if (left_visible_ && left_panel_ != nullptr && left_renderer != nullptr)
            {
                ImGui::SetCursorScreenPos(top_origin);
                left_panel_->BeginPanel(resolved_left, upper_height);
                left_renderer(left_context, *left_panel_, resolved_left, upper_height, fonts);
                left_panel_->EndPanel();
            }
            if (center_visible_ && center_panel_ != nullptr && center_renderer != nullptr)
            {
                const float center_x = top_origin.x +
                    (left_visible_ ? resolved_left + splitter_thickness : 0.0f);
                ImGui::SetCursorScreenPos(ImVec2(center_x, top_origin.y));
                center_panel_->BeginPanel(center_width, upper_height);
                center_renderer(center_context, *center_panel_, center_width, upper_height, fonts);
                center_panel_->EndPanel();
            }
            if (right_visible_ && right_panel_ != nullptr && right_renderer != nullptr)
            {
                const float right_x = top_origin.x + resolved_left + splitter_thickness
                                    + center_width + splitter_thickness;
                ImGui::SetCursorScreenPos(ImVec2(right_x, top_origin.y));
                right_panel_->BeginPanel(resolved_right, upper_height);
                right_renderer(right_context, *right_panel_, resolved_right, upper_height, fonts);
                right_panel_->EndPanel();
            }
        }

        ImGui::SetCursorScreenPos(ImVec2(top_origin.x, top_origin.y + upper_height + splitter_thickness));
        bottom_panel_->BeginPanel(region_size.x, bottom_height_);
        bottom_renderer(bottom_context, *bottom_panel_, region_size.x, bottom_height_, fonts);
        bottom_panel_->EndPanel();
    }
}

void DockLayout::RenderSplitterHorizontal(Splitter& splitter, float splitter_thickness)
{
    (void)splitter;
    (void)splitter_thickness;
}

void DockLayout::RenderSplitterVertical(Splitter& splitter, float splitter_thickness)
{
    (void)splitter;
    (void)splitter_thickness;
}

float DockLayout::ClampLeft(float width, float total_width) const noexcept
{
    if (width < left_min_)
    {
        width = left_min_;
    }
    const float max_left = total_width - center_min_ - right_min_ - 2.0f * style_.splitter_thickness;
    if (max_left < left_min_)
    {
        return left_min_;
    }
    return width < max_left ? width : max_left;
}

float DockLayout::ClampRight(float width, float total_width) const noexcept
{
    if (width < right_min_)
    {
        width = right_min_;
    }
    const float max_right = total_width - center_min_ - left_min_ - 2.0f * style_.splitter_thickness;
    if (max_right < right_min_)
    {
        return right_min_;
    }
    return width < max_right ? width : max_right;
}

float DockLayout::ClampBottom(float height, float total_height) const noexcept
{
    if (height < bottom_min_)
    {
        height = bottom_min_;
    }
    const float max_bottom = total_height - center_min_ - style_.splitter_thickness;
    if (max_bottom < bottom_min_)
    {
        return bottom_min_;
    }
    return height < max_bottom ? height : max_bottom;
}

} // namespace ellindyer::ui::layout
