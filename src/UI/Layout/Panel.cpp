#include "UI/Layout/Panel.hpp"

#include <utility>

namespace ellindyer::ui::layout
{

Panel::Panel(std::string name, std::string title)
    : name_(std::move(name))
    , title_(std::move(title))
{
}

const std::string& Panel::GetName() const noexcept
{
    return name_;
}

const std::string& Panel::GetTitle() const noexcept
{
    return title_;
}

void Panel::SetTitle(std::string title)
{
    title_ = std::move(title);
}

void Panel::SetStyle(const PanelStyle& style)
{
    style_ = style;
}

const PanelStyle& Panel::GetStyle() const noexcept
{
    return style_;
}

void Panel::SetMinWidth(float min_width) noexcept
{
    min_width_ = min_width;
}

void Panel::SetMinHeight(float min_height) noexcept
{
    min_height_ = min_height;
}

float Panel::GetMinWidth() const noexcept
{
    return min_width_;
}

float Panel::GetMinHeight() const noexcept
{
    return min_height_;
}

void Panel::BeginPanel(float width, float height)
{
    if (width < min_width_)
    {
        width = min_width_;
    }
    if (height < min_height_)
    {
        height = min_height_;
    }

    const std::string child_name = "##Panel_" + name_;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, style_.background_color);
    ImGui::PushStyleColor(ImGuiCol_Border,  style_.border_color);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, style_.border_size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2(style_.padding_x, style_.padding_y));

    ImGui::BeginChild(child_name.c_str(),
                      ImVec2(width, height),
                      true,
                      ImGuiWindowFlags_NoScrollbar);

    is_open_ = true;
}

void Panel::EndPanel()
{
    if (!is_open_)
    {
        return;
    }

    ImGui::EndChild();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);

    is_open_ = false;
}

void Panel::RenderHeader(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (!style_.show_header)
    {
        return;
    }

    if (fonts.small_regular != nullptr)
    {
        ImGui::PushFont(fonts.small_regular, fonts.small_regular->LegacySize);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, style_.title_color);
    ImGui::TextUnformatted(title_.c_str());
    ImGui::PopStyleColor();

    if (fonts.small_regular != nullptr)
    {
        ImGui::PopFont();
    }

    ImGui::Separator();
    ImGui::Spacing();
}

void Panel::RenderText(const ellindyer::ui::fonts::FontSet& fonts,
                       const std::string& text)
{
    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    ImGui::TextUnformatted(text.c_str());

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }
}

void Panel::RenderTextMuted(const ellindyer::ui::fonts::FontSet& fonts,
                            const std::string& text)
{
    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, style_.text_color);
    ImGui::TextUnformatted(text.c_str());
    ImGui::PopStyleColor();

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }
}

void Panel::RenderSeparator()
{
    ImGui::Separator();
}

} // namespace ellindyer::ui::layout
