#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Project/Project.hpp"
#include "UI/Fonts/FontManager.hpp"
#include "UI/Inspector/PropertyGrid.hpp"

namespace ellindyer::ui::inspector
{

struct ProjectPropertiesInspectorCallbacks
{
    std::function<void()> on_modified;
    std::function<void()> on_save_requested;
};

class ProjectPropertiesInspector
{
public:
    ProjectPropertiesInspector();
    ~ProjectPropertiesInspector();

    ProjectPropertiesInspector(const ProjectPropertiesInspector&) = delete;
    ProjectPropertiesInspector& operator=(const ProjectPropertiesInspector&) = delete;
    ProjectPropertiesInspector(ProjectPropertiesInspector&&) noexcept = delete;
    ProjectPropertiesInspector& operator=(ProjectPropertiesInspector&&) noexcept = delete;

    void SetCallbacks(const ProjectPropertiesInspectorCallbacks& callbacks);

    void Bind(ellindyer::project::Project* project);

    void Unbind();

    [[nodiscard]] bool IsBound() const noexcept;

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

private:
    void RebuildFields();

    ellindyer::project::Project*              project_ = nullptr;
    ProjectPropertiesInspectorCallbacks       callbacks_{};
    PropertyGrid                              grid_{};
};

} // namespace ellindyer::ui::inspector
