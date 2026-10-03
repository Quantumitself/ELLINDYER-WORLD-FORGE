#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Inspector/PropertyField.hpp"

namespace ellindyer::ui::inspector
{

struct PropertyGridStyle
{
    float   label_column_width  = 140.0f;
    float   row_spacing_y       = 4.0f;
    float   padding_x           = 6.0f;
    float   padding_y           = 4.0f;
    bool    draw_separators     = true;
};

class PropertyGrid
{
public:
    PropertyGrid();
    ~PropertyGrid();

    PropertyGrid(const PropertyGrid&) = delete;
    PropertyGrid& operator=(const PropertyGrid&) = delete;
    PropertyGrid(PropertyGrid&&) noexcept = delete;
    PropertyGrid& operator=(PropertyGrid&&) noexcept = delete;

    void SetStyle(const PropertyGridStyle& style);

    [[nodiscard]] const PropertyGridStyle& GetStyle() const noexcept;

    void Clear();

    void AddField(PropertyField field);

    [[nodiscard]] std::size_t GetFieldCount() const noexcept;

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

    [[nodiscard]] bool HasEdits() const noexcept;

    void ClearEditFlag() noexcept;

private:
    void RenderField(const PropertyField& field,
                     const ellindyer::ui::fonts::FontSet& fonts,
                     std::size_t index);

    PropertyGridStyle          style_{};
    std::vector<PropertyField> fields_;
    bool                       has_edits_ = false;
};

} // namespace ellindyer::ui::inspector
