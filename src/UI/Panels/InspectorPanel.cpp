#include "UI/Panels/InspectorPanel.hpp"

#include <imgui.h>

#include <utility>

#include "UI/ImGui/ImGuiCompat.hpp"

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

    switch (mode_)
    {
    case InspectorMode::Empty:
    {
        panel.RenderTextMuted(fonts, "Nothing selected.");
        panel.RenderTextMuted(fonts, "Select an object to inspect its properties.");
        break;
    }

    case InspectorMode::ProjectProperties:
    {
        RenderProjectProperties(fonts);
        break;
    }

    case InspectorMode::Selection:
    {
        RenderSelection(fonts);
        break;
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

void InspectorPanel::ShowProjectProperties(ellindyer::project::Project* project)
{
    project_inspector_.Bind(project);
    mode_ = InspectorMode::ProjectProperties;
}

void InspectorPanel::ShowSelection()
{
    project_inspector_.Unbind();
    mode_ = InspectorMode::Selection;
}

InspectorMode InspectorPanel::GetMode() const noexcept
{
    return mode_;
}

ellindyer::ui::inspector::ProjectPropertiesInspector&
InspectorPanel::GetProjectPropertiesInspector() noexcept
{
    return project_inspector_;
}

const ellindyer::ui::inspector::ProjectPropertiesInspector&
InspectorPanel::GetProjectPropertiesInspector() const noexcept
{
    return project_inspector_;
}

void InspectorPanel::SetPropertyChangedHandler(PropertyChangedHandler handler)
{
    property_changed_handler_ = std::move(handler);
}

void InspectorPanel::RenderSelection(const ellindyer::ui::fonts::FontSet& fonts)
{
    using ellindyer::ui::layout::Panel;
    using ellindyer::ui::layout::PanelStyle;

    Panel inner("InspectorSelection", "");

    PanelStyle style{};
    style.show_header = false;
    inner.SetStyle(style);
    inner.BeginPanel(ImGui::GetContentRegionAvail().x,
                     ImGui::GetContentRegionAvail().y);

    if (selection_title_.empty())
    {
        inner.RenderTextMuted(fonts, "Nothing selected.");
        inner.RenderTextMuted(fonts, "Select an object to inspect its properties.");
        inner.EndPanel();
        return;
    }

    inner.RenderText(fonts, selection_title_);

    if (!selection_subtitle_.empty())
    {
        inner.RenderTextMuted(fonts, selection_subtitle_);
    }

    inner.RenderSeparator();

    if (properties_.empty())
    {
        inner.RenderTextMuted(fonts, "(no properties)");
    }
    else
    {
        for (const PropertyEntry& entry : properties_)
        {
            if (fonts.small_regular != nullptr)
            {
                ellindyer::ui::imgui_compat::PushFont(fonts.small_regular);
            }
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.00f));
            ImGui::TextUnformatted(entry.key.c_str());
            ImGui::PopStyleColor();
            if (fonts.small_regular != nullptr)
            {
                ImGui::PopFont();
            }

            inner.RenderText(fonts, entry.value);
        }
    }

    inner.EndPanel();
}

void InspectorPanel::RenderProjectProperties(const ellindyer::ui::fonts::FontSet& fonts)
{
    project_inspector_.SetCallbacks(
        ellindyer::ui::inspector::ProjectPropertiesInspectorCallbacks{
            [this]()
            {
                if (property_changed_handler_)
                {
                    property_changed_handler_();
                }
            },
            nullptr});

    project_inspector_.Render(fonts);
}

} // namespace ellindyer::ui::panels
