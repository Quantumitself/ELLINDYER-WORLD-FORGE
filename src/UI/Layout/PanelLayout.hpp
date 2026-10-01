#pragma once

#include <cstdint>

#include <imgui.h>

#include "UI/Layout/Panel.hpp"

namespace ellindyer::ui::layout
{

struct PanelLayoutMetrics
{
    float left_width    = 0.0f;
    float center_width  = 0.0f;
    float right_width   = 0.0f;
    float panel_height  = 0.0f;
    float spacing       = 0.0f;
};

struct PanelLayoutConfiguration
{
    float left_width_fraction  = 0.20f;
    float right_width_fraction = 0.22f;
    float left_min_width       = 220.0f;
    float right_min_width      = 260.0f;
    float center_min_width     = 320.0f;
    float panel_spacing        = 8.0f;
    float vertical_reserved    = 0.0f;
};

class PanelLayout
{
public:
    PanelLayout();
    ~PanelLayout();

    PanelLayout(const PanelLayout&) = delete;
    PanelLayout& operator=(const PanelLayout&) = delete;
    PanelLayout(PanelLayout&&) noexcept = delete;
    PanelLayout& operator=(PanelLayout&&) noexcept = delete;

    void Configure(const PanelLayoutConfiguration& configuration);

    [[nodiscard]] const PanelLayoutConfiguration& GetConfiguration() const noexcept;

    [[nodiscard]] PanelLayoutMetrics ComputeMetrics(const ImVec2& available) const;

private:
    PanelLayoutConfiguration configuration_{};
};

} // namespace ellindyer::ui::layout
