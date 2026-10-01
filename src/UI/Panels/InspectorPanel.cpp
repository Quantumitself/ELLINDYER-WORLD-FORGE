#include "UI/Panels/InspectorPanel.hpp"

#include <imgui.h>

namespace ellindyer::ui::panels
{

InspectorPanel::InspectorPanel() = default;

InspectorPanel::~InspectorPanel() = default;

void InspectorPanel::Render(
    const ellindyer::ui::fonts::FontSet& fonts,
    const ellindyer::ui::layout::PanelLayoutMetrics& metrics)
{
    using ellindyer::ui::layout::Panel;
    using ellindyer::ui::layout::PanelStyle;

    Panel panel("Inspector", "Inspector");

    PanelStyle style{};
    panel.SetStyle(style);
    panel.SetMinWidth(metrics.right_width);

    panel.BeginPanel(metrics.right_width, metrics.panel_height);

    panel.RenderHeader(fonts);

    if (selection_title_.empty())
    {
        panel.RenderTextMuted(fonts, "Nothing selected.");
        panel.RenderTextMuted(fonts, "Select an object to inspect its properties.");
    }
    else
    {
        panel.RenderText(fonts, selection_title_);

        if (!selection_subtitle_.empty())
        {
            panel.RenderTextMuted(fonts, selection_subtitle_);
        }

        panel.RenderSeparator();

        if (properties_.empty())
        {
            panel.RenderTextMuted(fonts, "(no properties)");
        }
        else
        {
            for (const PropertyEntry& entry : properties_)
            {
                panel.RenderTextMuted(fonts, entry.key);
                panel.RenderText(fonts, "  " + entry.value);
            }
        }
    }

    panel.EndPanel();
}

void InspectorPanel::SetSelectionTitle(std::string title)
{
    selection_title_ = std::move(title);
}

void InspectorPanel::SetSelectionSubtitle(std::string subtitle)
{
    selection_subtitle_ = std::move(subtitle);
}

void InspectorPanel::AddSection(std::string section_title)
{
    if (section_title.empty())
    {
        return;
    }
    sections_.push_back(std::move(section_title));
}

void InspectorPanel::AddProperty(std::string key, std::string value)
{
    if (key.empty())
    {
        return;
    }
    properties_.push_back(PropertyEntry{std::move(key), std::move(value)});
}

void InspectorPanel::Clear()
{
    selection_title_.clear();
    selection_subtitle_.clear();
    sections_.clear();
    properties_.clear();
}

} // namespace ellindyer::ui::panels
