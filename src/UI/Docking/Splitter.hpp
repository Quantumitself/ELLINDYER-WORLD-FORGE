#pragma once

#include <cstdint>

#include <imgui.h>

namespace ellindyer::ui::docking
{

enum class SplitterOrientation : std::uint8_t
{
    Vertical,
    Horizontal
};

struct SplitterStyle
{
    float thickness           = 6.0f;
    float hit_padding         = 3.0f;
    float min_left_size       = 120.0f;
    float min_right_size      = 120.0f;
    ImVec4 idle_color         = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    ImVec4 hover_color        = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    ImVec4 active_color       = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
};

class Splitter
{
public:
    Splitter() = default;
    explicit Splitter(SplitterStyle style);

    Splitter(const Splitter&) = delete;
    Splitter& operator=(const Splitter&) = delete;
    Splitter(Splitter&&) noexcept = default;
    Splitter& operator=(Splitter&&) noexcept = default;

    void SetStyle(const SplitterStyle& style);

    [[nodiscard]] const SplitterStyle& GetStyle() const noexcept;

    void SetOrientation(SplitterOrientation orientation) noexcept;

    [[nodiscard]] SplitterOrientation GetOrientation() const noexcept;

    void SetIdentifier(const char* identifier);

    [[nodiscard]] const char* GetIdentifier() const noexcept;

    void SetBounds(float minimum, float maximum) noexcept;

    [[nodiscard]] float GetValue() const noexcept;

    void SetValue(float value) noexcept;

    [[nodiscard]] bool IsHovered() const noexcept;

    [[nodiscard]] bool IsActive() const noexcept;

    [[nodiscard]] bool IsDragged() const noexcept;

    [[nodiscard]] bool Render(float length);

    [[nodiscard]] float GetThickness() const noexcept;

private:
    void DrawBar(float length, const ImVec2& cursor, bool hovered, bool active);

    SplitterStyle       style_{};
    SplitterOrientation orientation_ = SplitterOrientation::Vertical;
    const char*         identifier_  = "##Splitter";
    float               value_       = 0.5f;
    float               minimum_     = 0.1f;
    float               maximum_     = 0.9f;
    bool                hovered_     = false;
    bool                active_      = false;
    bool                dragged_     = false;
};

} // namespace ellindyer::ui::docking
