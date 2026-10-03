#pragma once

#include <functional>
#include <string>
#include <vector>

#include "Project/Project.hpp"
#include "UI/Fonts/FontManager.hpp"
#include "UI/Inspector/ProjectPropertiesInspector.hpp"
#include "UI/Layout/Panel.hpp"
#include "UI/Layout/PanelLayout.hpp"

namespace ellindyer::ui::panels
{

enum class InspectorMode
{
    Empty,
    ProjectProperties,
    Selection
};

class InspectorPanel
{
public:
    using PropertyChangedHandler = std::function<void()>;

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

    void ShowProjectProperties(ellindyer::project::Project* project);

    void ShowSelection();

    [[nodiscard]] InspectorMode GetMode() const noexcept;

    [[nodiscard]] ellindyer::ui::inspector::ProjectPropertiesInspector&
        GetProjectPropertiesInspector() noexcept;

    [[nodiscard]] const ellindyer::ui::inspector::ProjectPropertiesInspector&
        GetProjectPropertiesInspector() const noexcept;

    void SetPropertyChangedHandler(PropertyChangedHandler handler);

private:
    struct PropertyEntry
    {
        std::string key;
        std::string value;
    };

    void RenderSelection(const ellindyer::ui::fonts::FontSet& fonts);

    void RenderProjectProperties(const ellindyer::ui::fonts::FontSet& fonts);

    std::string                selection_title_;
    std::string                selection_subtitle_;
    std::vector<std::string>   sections_;
    std::vector<PropertyEntry> properties_;
    InspectorMode              mode_ = InspectorMode::Empty;

    ellindyer::ui::inspector::ProjectPropertiesInspector project_inspector_;
    PropertyChangedHandler     property_changed_handler_;
};

} // namespace ellindyer::ui::panels
