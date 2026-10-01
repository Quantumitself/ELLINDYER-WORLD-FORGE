#include "UI/Shell/ShellLayout.hpp"

#include <algorithm>

namespace ellindyer::ui::shell
{

namespace
{

constexpr float kPanelSpacing = 8.0f;

ImVec4 ColorPrimary()   { return ImVec4(1.00f, 1.00f, 1.00f, 1.00f); }
ImVec4 ColorSecondary() { return ImVec4(0.68f, 0.68f, 0.68f, 1.00f); }
ImVec4 ColorMuted()     { return ImVec4(0.45f, 0.45f, 0.45f, 1.00f); }
ImVec4 ColorPanelBg()   { return ImVec4(0.09f, 0.09f, 0.09f, 1.00f); }
ImVec4 ColorPanelBorder() { return ImVec4(0.22f, 0.22f, 0.22f, 1.00f); }

void RenderPanelHeader(const ellindyer::ui::fonts::FontSet& fonts,
                       const char* title)
{
        if (fonts.small_regular != nullptr)
        {
            ImGui::PushFont(fonts.small_regular, fonts.small_regular->LegacySize);
        }

    ImGui::PushStyleColor(ImGuiCol_Text, ColorSecondary());
    ImGui::TextUnformatted(title);
    ImGui::PopStyleColor();

    if (fonts.small_regular != nullptr)
    {
        ImGui::PopFont();
    }

    ImGui::Separator();
    ImGui::Spacing();
}

void RenderPlaceholderLine(const ellindyer::ui::fonts::FontSet& fonts,
                           const char* text)
{
        if (fonts.default_regular != nullptr)
        {
            ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
        }

    ImGui::PushStyleColor(ImGuiCol_Text, ColorMuted());
    ImGui::TextUnformatted(text);
    ImGui::PopStyleColor();

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }
}

void RenderPanelBackground(float padding_x, float padding_y)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ColorPanelBg());
    ImGui::PushStyleColor(ImGuiCol_Border,  ColorPanelBorder());
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding_x, padding_y));
}

void PopPanelBackground()
{
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

} // namespace

ShellLayout::ShellLayout() = default;

ShellLayout::~ShellLayout() = default;

void ShellLayout::Configure(const ShellLayoutConfiguration& configuration)
{
    configuration_ = configuration;
}

void ShellLayout::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    const ImVec2 available = ImGui::GetContentRegionAvail();
    ComputePanelWidths(available);

    content_height_ = available.y;

    RenderLeftPanel(fonts);

    ImGui::SameLine(0.0f, kPanelSpacing);

    RenderCenterPanel(fonts);

    ImGui::SameLine(0.0f, kPanelSpacing);

    RenderRightPanel(fonts);
}

float ShellLayout::GetLeftPanelWidth() const noexcept
{
    return left_panel_width_;
}

float ShellLayout::GetRightPanelWidth() const noexcept
{
    return right_panel_width_;
}

float ShellLayout::GetCenterWidth() const noexcept
{
    return center_width_;
}

float ShellLayout::GetContentHeight() const noexcept
{
    return content_height_;
}

void ShellLayout::RenderLeftPanel(const ellindyer::ui::fonts::FontSet& fonts)
{
    RenderPanelBackground(10.0f, 10.0f);

    ImGui::BeginChild("##ShellLeftPanel",
                      ImVec2(left_panel_width_, content_height_),
                      true,
                      ImGuiWindowFlags_NoScrollbar);

    RenderPanelHeader(fonts, "Project Explorer");

    RenderPlaceholderLine(fonts, "No project loaded.");
    RenderPlaceholderLine(fonts, "Project tree appears once a project is opened.");

    ImGui::EndChild();
    PopPanelBackground();
}

void ShellLayout::RenderCenterPanel(const ellindyer::ui::fonts::FontSet& fonts)
{
    RenderPanelBackground(12.0f, 12.0f);

    ImGui::BeginChild("##ShellCenterPanel",
                      ImVec2(center_width_, content_height_),
                      true,
                      ImGuiWindowFlags_NoScrollbar);

    RenderPanelHeader(fonts, "Workspace");

        if (fonts.large_regular != nullptr)
        {
            ImGui::PushFont(fonts.large_regular, fonts.large_regular->LegacySize);
        }

    ImGui::PushStyleColor(ImGuiCol_Text, ColorPrimary());
    ImGui::TextUnformatted("Ellindyer World Forge");
    ImGui::PopStyleColor();

    if (fonts.large_regular != nullptr)
    {
        ImGui::PopFont();
    }

    ImGui::Spacing();

    RenderPlaceholderLine(fonts,
        "The workspace hosts schema, entity, relationship, narrative, spatial,");
    RenderPlaceholderLine(fonts,
        "formula, rule, and validation surfaces as they become available.");

    ImGui::EndChild();
    PopPanelBackground();
}

void ShellLayout::RenderRightPanel(const ellindyer::ui::fonts::FontSet& fonts)
{
    RenderPanelBackground(10.0f, 10.0f);

    ImGui::BeginChild("##ShellRightPanel",
                      ImVec2(right_panel_width_, content_height_),
                      true,
                      ImGuiWindowFlags_NoScrollbar);

    RenderPanelHeader(fonts, "Inspector");

    RenderPlaceholderLine(fonts, "Nothing selected.");
    RenderPlaceholderLine(fonts, "Properties, relationships, assets, and");
    RenderPlaceholderLine(fonts, "diagnostics appear here when something is selected.");

    ImGui::EndChild();
    PopPanelBackground();
}

void ShellLayout::ComputePanelWidths(const ImVec2& content_size)
{
    const float total_width = content_size.x - (2.0f * kPanelSpacing);
    if (total_width <= 0.0f)
    {
        left_panel_width_  = 0.0f;
        right_panel_width_ = 0.0f;
        center_width_      = 0.0f;
        return;
    }

    float left  = content_size.x * configuration_.left_panel_width_fraction;
    float right = content_size.x * configuration_.right_panel_width_fraction;

    left  = std::max(left,  configuration_.left_panel_min_width);
    right = std::max(right, configuration_.right_panel_min_width);

    float center = total_width - left - right;

    if (center < configuration_.center_min_width)
    {
        const float deficit = configuration_.center_min_width - center;
        const float shrinkable_left  = std::max(0.0f, left  - configuration_.left_panel_min_width);
        const float shrinkable_right = std::max(0.0f, right - configuration_.right_panel_min_width);
        const float shrinkable_total = shrinkable_left + shrinkable_right;

        if (shrinkable_total > 0.0f)
        {
            const float left_shrink  = deficit * (shrinkable_left  / shrinkable_total);
            const float right_shrink = deficit * (shrinkable_right / shrinkable_total);
            left  = std::max(configuration_.left_panel_min_width,  left  - left_shrink);
            right = std::max(configuration_.right_panel_min_width, right - right_shrink);
        }

        center = total_width - left - right;
        if (center < 0.0f)
        {
            center = 0.0f;
        }
    }

    left_panel_width_  = left;
    right_panel_width_ = right;
    center_width_      = center;
}

} // namespace ellindyer::ui::shell
