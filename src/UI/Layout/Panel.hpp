#pragma once

#include <cstdint>
#include <string>

#include <imgui.h>

#include "UI/Fonts/FontManager.hpp"

namespace ellindyer::ui::layout
{

enum class PanelPlacement : std::uint8_t
{
    Left,
    Center,
    Right,
    Bottom,
    Top
};

struct PanelStyle
{
    ImVec4  background_color = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    ImVec4  border_color     = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    ImVec4  title_color      = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
    ImVec4  text_color       = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    float   border_size      = 1.0f;
    float   padding_x        = 10.0f;
    float   padding_y        = 10.0f;
    bool    show_header      = true;
};

class Panel
{
public:
    Panel() = default;
    Panel(std::string name, std::string title);

    Panel(const Panel&) = delete;
    Panel& operator=(const Panel&) = delete;
    Panel(Panel&&) noexcept = default;
    Panel& operator=(Panel&&) noexcept = default;

    [[nodiscard]] const std::string& GetName() const noexcept;

    [[nodiscard]] const std::string& GetTitle() const noexcept;

    void SetTitle(std::string title);

    void SetStyle(const PanelStyle& style);

    [[nodiscard]] const PanelStyle& GetStyle() const noexcept;

    void SetMinWidth(float min_width) noexcept;
    void SetMinHeight(float min_height) noexcept;

    [[nodiscard]] float GetMinWidth() const noexcept;
    [[nodiscard]] float GetMinHeight() const noexcept;

    void BeginPanel(float width, float height);

    void EndPanel();

    void RenderHeader(const ellindyer::ui::fonts::FontSet& fonts);

    void RenderText(const ellindyer::ui::fonts::FontSet& fonts,
                    const std::string& text);

    void RenderTextMuted(const ellindyer::ui::fonts::FontSet& fonts,
                         const std::string& text);

    void RenderSeparator();

private:
    std::string name_;
    std::string title_;
    PanelStyle  style_{};
    float       min_width_  = 0.0f;
    float       min_height_ = 0.0f;
    bool        is_open_    = false;
};

} // namespace ellindyer::ui::layout
