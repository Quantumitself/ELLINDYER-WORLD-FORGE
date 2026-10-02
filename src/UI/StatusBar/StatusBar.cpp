#include "UI/StatusBar/StatusBar.hpp"

#include <cstdio>

#include <imgui.h>

#include <utility>

namespace ellindyer::ui::statusbar
{

namespace
{

constexpr ImGuiWindowFlags kStatusBarFlags =
    ImGuiWindowFlags_NoScrollbar
    | ImGuiWindowFlags_NoScrollWithMouse
    | ImGuiWindowFlags_NoBackground;

ImVec4 ColorMuted()     { return ImVec4(0.45f, 0.45f, 0.45f, 1.00f); }
ImVec4 ColorSeparator() { return ImVec4(0.30f, 0.30f, 0.30f, 1.00f); }
ImVec4 ColorAccent()    { return ImVec4(0.85f, 0.85f, 0.85f, 1.00f); }
ImVec4 ColorTrack()     { return ImVec4(0.22f, 0.22f, 0.22f, 1.00f); }

constexpr float kProgressBarWidth  = 96.0f;
constexpr float kProgressBarHeight = 4.0f;

} // namespace

StatusBar::StatusBar() = default;

StatusBar::~StatusBar() = default;

void StatusBar::Clear()
{
    segments_.clear();
}

void StatusBar::SetStyle(const StatusBarStyle& style)
{
    style_ = style;
}

const StatusBarStyle& StatusBar::GetStyle() const noexcept
{
    return style_;
}

void StatusBar::AddSegment(StatusBarSegment segment)
{
    segments_.push_back(std::move(segment));
}

void StatusBar::AddText(std::string identifier,
                        std::string text,
                        std::string tooltip)
{
    segments_.push_back(MakeTextSegment(std::move(identifier),
                                        std::move(text),
                                        std::move(tooltip)));
}

void StatusBar::AddMessage(std::string identifier, std::string text)
{
    segments_.push_back(MakeMessageSegment(std::move(identifier),
                                           std::move(text)));
}

void StatusBar::AddCounter(std::string identifier,
                           std::string label,
                           std::uint64_t value)
{
    segments_.push_back(MakeCounterSegment(std::move(identifier),
                                           std::move(label),
                                           value));
}

void StatusBar::AddProgress(std::string identifier,
                            float progress_value,
                            std::string tooltip)
{
    segments_.push_back(MakeProgressSegment(std::move(identifier),
                                            progress_value,
                                            std::move(tooltip)));
}

void StatusBar::AddSeparator(std::string identifier)
{
    segments_.push_back(MakeSeparatorSegment(std::move(identifier)));
}

void StatusBar::AddSpacer(std::string identifier, float width)
{
    segments_.push_back(MakeSpacerSegment(std::move(identifier), width));
}

void StatusBar::SetSegmentText(const std::string& identifier, std::string text)
{
    StatusBarSegment* segment = FindSegmentMutable(identifier);
    if (segment != nullptr)
    {
        ::ellindyer::ui::statusbar::SetSegmentText(*segment, std::move(text));
    }
}

void StatusBar::SetSegmentCounter(const std::string& identifier,
                                  std::string label,
                                  std::uint64_t value)
{
    StatusBarSegment* segment = FindSegmentMutable(identifier);
    if (segment != nullptr)
    {
        ::ellindyer::ui::statusbar::SetSegmentCounter(*segment, std::move(label), value);
    }
}

void StatusBar::SetSegmentProgress(const std::string& identifier, float progress_value)
{
    StatusBarSegment* segment = FindSegmentMutable(identifier);
    if (segment != nullptr)
    {
        ::ellindyer::ui::statusbar::SetSegmentProgress(*segment, progress_value);
    }
}

void StatusBar::SetSegmentVisible(const std::string& identifier, bool visible)
{
    StatusBarSegment* segment = FindSegmentMutable(identifier);
    if (segment != nullptr)
    {
        ::ellindyer::ui::statusbar::SetSegmentVisible(*segment, visible);
    }
}

void StatusBar::SetSegmentClickedHandler(SegmentClickedHandler handler)
{
    clicked_handler_ = std::move(handler);
}

void StatusBar::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2(style_.padding_x, style_.padding_y));

    ImGui::BeginChild("##MainStatusBar",
                      ImVec2(0.0f, style_.height),
                      false,
                      kStatusBarFlags);

    if (fonts.small_regular != nullptr)
    {
        ImGui::PushFont(fonts.small_regular, fonts.small_regular->LegacySize);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());

    bool first_rendered = true;

    for (const StatusBarSegment& segment : segments_)
    {
        if (!segment.visible)
        {
            continue;
        }
        RenderSegment(segment, fonts, first_rendered);
    }

    ImGui::PopStyleColor();

    if (fonts.small_regular != nullptr)
    {
        ImGui::PopFont();
    }

    ImGui::EndChild();

    ImGui::PopStyleVar();
}

float StatusBar::GetHeight() const noexcept
{
    return style_.height;
}

std::size_t StatusBar::GetSegmentCount() const noexcept
{
    return segments_.size();
}

const StatusBarSegment* StatusBar::FindSegment(const std::string& identifier) const noexcept
{
    if (identifier.empty())
    {
        return nullptr;
    }

    for (const StatusBarSegment& segment : segments_)
    {
        if (segment.identifier == identifier)
        {
            return &segment;
        }
    }
    return nullptr;
}

void StatusBar::RenderSegment(const StatusBarSegment& segment,
                              const ellindyer::ui::fonts::FontSet& /*fonts*/,
                              bool& first_rendered)
{
    switch (segment.kind)
    {
    case StatusBarSegmentKind::Spacer:
    {
        if (!first_rendered)
        {
            ImGui::SameLine();
        }
        ImGui::Dummy(ImVec2(segment.spacer_width, 1.0f));
        first_rendered = false;
        return;
    }

    case StatusBarSegmentKind::Separator:
    {
        if (!first_rendered)
        {
            ImGui::SameLine();
        }

        const ImVec2 cursor = ImGui::GetCursorScreenPos();
        const float height = ImGui::GetTextLineHeight();

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddLine(ImVec2(cursor.x + style_.separator_width * 0.5f, cursor.y),
                           ImVec2(cursor.x + style_.separator_width * 0.5f, cursor.y + height),
                           ImGui::GetColorU32(ColorSeparator()));

        ImGui::Dummy(ImVec2(style_.separator_width, height));

        first_rendered = false;
        return;
    }

    case StatusBarSegmentKind::Text:
    case StatusBarSegmentKind::Message:
    {
        if (!first_rendered)
        {
            ImGui::SameLine();
        }

        ImGui::TextUnformatted(segment.text.c_str());

        if (!segment.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(segment.tooltip.c_str());
            ImGui::EndTooltip();
        }

        first_rendered = false;
        return;
    }

    case StatusBarSegmentKind::Counter:
    {
        if (!first_rendered)
        {
            ImGui::SameLine();
        }

        const std::uint64_t value = static_cast<std::uint64_t>(segment.progress_value);

        char buffer[128] = {};
        std::snprintf(buffer,
                      sizeof(buffer),
                      "%s: %llu",
                      segment.text.c_str(),
                      static_cast<unsigned long long>(value));

        ImGui::TextUnformatted(buffer);

        if (!segment.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(segment.tooltip.c_str());
            ImGui::EndTooltip();
        }

        first_rendered = false;
        return;
    }

    case StatusBarSegmentKind::Progress:
    {
        if (!first_rendered)
        {
            ImGui::SameLine();
        }

        const ImVec2 cursor = ImGui::GetCursorScreenPos();
        const float height = ImGui::GetTextLineHeight();
        const float bar_y = cursor.y + (height - kProgressBarHeight) * 0.5f;

        const ImVec2 bar_min(cursor.x, bar_y);
        const ImVec2 bar_max(cursor.x + kProgressBarWidth, bar_y + kProgressBarHeight);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(bar_min, bar_max, ImGui::GetColorU32(ColorTrack()));

        float clamped = segment.progress_value;
        if (clamped < 0.0f) clamped = 0.0f;
        if (clamped > 1.0f) clamped = 1.0f;

        const float filled_width = kProgressBarWidth * clamped;
        if (filled_width > 0.0f)
        {
            const ImVec2 filled_max(bar_min.x + filled_width, bar_max.y);
            draw_list->AddRectFilled(bar_min, filled_max, ImGui::GetColorU32(ColorAccent()));
        }

        ImGui::InvisibleButton(segment.identifier.c_str(),
                               ImVec2(kProgressBarWidth, height));

        if (ImGui::IsItemClicked())
        {
            if (clicked_handler_ && !segment.identifier.empty())
            {
                clicked_handler_(segment.identifier);
            }
        }

        if (!segment.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(segment.tooltip.c_str());
            ImGui::EndTooltip();
        }

        first_rendered = false;
        return;
    }
    }
}

StatusBarSegment* StatusBar::FindSegmentMutable(const std::string& identifier)
{
    if (identifier.empty())
    {
        return nullptr;
    }

    for (StatusBarSegment& segment : segments_)
    {
        if (segment.identifier == identifier)
        {
            return &segment;
        }
    }
    return nullptr;
}

} // namespace ellindyer::ui::statusbar
