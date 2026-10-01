#include "UI/Layout/PanelLayout.hpp"

#include <algorithm>

namespace ellindyer::ui::layout
{

PanelLayout::PanelLayout() = default;

PanelLayout::~PanelLayout() = default;

void PanelLayout::Configure(const PanelLayoutConfiguration& configuration)
{
    configuration_ = configuration;
}

const PanelLayoutConfiguration& PanelLayout::GetConfiguration() const noexcept
{
    return configuration_;
}

PanelLayoutMetrics PanelLayout::ComputeMetrics(const ImVec2& available) const
{
    PanelLayoutMetrics metrics{};
    metrics.spacing = configuration_.panel_spacing;

    const float usable_width = available.x - (configuration_.panel_spacing * 2.0f);
    const float usable_height = available.y - configuration_.vertical_reserved;

    metrics.panel_height = usable_height > 0.0f ? usable_height : 0.0f;

    if (usable_width <= 0.0f)
    {
        metrics.left_width   = 0.0f;
        metrics.center_width = 0.0f;
        metrics.right_width  = 0.0f;
        return metrics;
    }

    float left  = available.x * configuration_.left_width_fraction;
    float right = available.x * configuration_.right_width_fraction;

    left  = std::max(left,  configuration_.left_min_width);
    right = std::max(right, configuration_.right_min_width);

    float center = usable_width - left - right;

    if (center < configuration_.center_min_width)
    {
        const float deficit = configuration_.center_min_width - center;
        const float shrink_left  = std::max(0.0f, left  - configuration_.left_min_width);
        const float shrink_right = std::max(0.0f, right - configuration_.right_min_width);
        const float shrink_total = shrink_left + shrink_right;

        if (shrink_total > 0.0f)
        {
            left  = std::max(configuration_.left_min_width,  left  - deficit * (shrink_left  / shrink_total));
            right = std::max(configuration_.right_min_width, right - deficit * (shrink_right / shrink_total));
        }

        center = usable_width - left - right;
        if (center < 0.0f)
        {
            center = 0.0f;
        }
    }

    metrics.left_width   = left;
    metrics.right_width  = right;
    metrics.center_width = center;

    return metrics;
}

} // namespace ellindyer::ui::layout
