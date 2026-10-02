#include "UI/Docking/DockPanel.hpp"

#include <imgui.h>

#include <utility>

namespace ellindyer::ui::docking
{

DockPanel::DockPanel(std::string name, std::string title)
    : name_(std::move(name))
    , title_(std::move(title))
{
}

const std::string& DockPanel::GetName() const noexcept
{
    return name_;
}

const std::string& DockPanel::GetTitle() const noexcept
{
    return title_;
}

void DockPanel::SetTitle(std::string title)
{
    title_ = std::move(title);
}

void DockPanel::SetStyle(const DockPanelStyle& style)
{
    style_ = style;
}

const DockPanelStyle& DockPanel::GetStyle() const noexcept
{
    return style_;
}

void DockPanel::SetContentRenderer(ContentRenderer renderer)
{
    content_renderer_ = std::move(renderer);
}

void DockPanel::SetHeaderRenderer(HeaderRenderer renderer)
{
    header_renderer_ = std::move(renderer);
}

void DockPanel::SetVisible(bool visible) noexcept
{
    visible_ = visible;
}

bool DockPanel::IsVisible() const noexcept
{
    return visible_;
}

void DockPanel::SetMinWidth(float min_width) noexcept
{
    min_width_ = min_width;
}

void DockPanel::SetMinHeight(float min_height) noexcept
{
    min_height_ = min_height;
}

float DockPanel::GetMinWidth() const noexcept
{
    return min_width_;
}

float DockPanel::GetMinHeight() const noexcept
{
    return min_height_;
}

void DockPanel::Render(float width, float height)
{
    if (!visible_)
    {
        return;
    }

    if (width < min_width_)
    {
        width = min_width_;
    }
    if (height < min_height_)
    {
        height = min_height_;
    }

    const std::string child_name = "##DockPanel_" + name_;

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

    if (!title_.empty())
    {
        if (header_renderer_)
        {
            header_renderer_();
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, style_.title_color);
            ImGui::TextUnformatted(title_.c_str());
            ImGui::PopStyleColor();
            ImGui::Separator();
            ImGui::Spacing();
        }
    }

    if (content_renderer_)
    {
        content_renderer_();
    }

    ImGui::EndChild();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

} // namespace ellindyer::ui::docking
