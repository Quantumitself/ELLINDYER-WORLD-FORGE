#pragma once

#include <string>
#include <vector>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Layout/Panel.hpp"
#include "UI/Layout/PanelLayout.hpp"

namespace ellindyer::ui::panels
{

class InspectorPanel
{
public:
    InspectorPanel();
    ~InspectorPanel();

    InspectorPanel(const InspectorPanel&) = delete;
    InspectorPanel& operator=(const InspectorPanel&) = delete;
    InspectorPanel(InspectorPanel&&) noexcept = delete;
    InspectorPanel& operator=(InspectorPanel&&) noexcept = delete;

    void Render(const ellindyer::ui::fonts::FontSet& fonts,
                const ellindyer::ui::layout::PanelLayoutMetrics& metrics);

    void SetSelectionTitle(std::string title);
    void SetSelectionSubtitle(std::string subtitle);
    void AddSection(std::string section_title);
    void AddProperty(std::string key, std::string value);
    void Clear();

private:
    struct PropertyEntry
    {
        std::string key;
        std::string value;
    };

    std::string                selection_title_;
    std::string                selection_subtitle_;
    std::vector<std::string>   sections_;
    std::vector<PropertyEntry> properties_;
};

} // namespace ellindyer::ui::panels
