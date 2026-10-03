#include "UI/Inspector/PropertyGrid.hpp"

#include <cstdio>
#include <cstring>

#include <imgui.h>

#include "UI/ImGui/ImGuiCompat.hpp"

namespace ellindyer::ui::inspector
{

namespace
{

constexpr std::size_t kTextBufferSize = 1024;

} // namespace

PropertyGrid::PropertyGrid() = default;

PropertyGrid::~PropertyGrid() = default;

void PropertyGrid::SetStyle(const PropertyGridStyle& style)
{
    style_ = style;
}

const PropertyGridStyle& PropertyGrid::GetStyle() const noexcept
{
    return style_;
}

void PropertyGrid::Clear()
{
    fields_.clear();
}

void PropertyGrid::AddField(PropertyField field)
{
    fields_.push_back(std::move(field));
}

std::size_t PropertyGrid::GetFieldCount() const noexcept
{
    return fields_.size();
}

void PropertyGrid::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        ImVec2(style_.padding_x, style_.row_spacing_y));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                        ImVec2(style_.padding_x, style_.padding_y));

    for (std::size_t index = 0; index < fields_.size(); ++index)
    {
        RenderField(fields_[index], fonts, index);
    }

    ImGui::PopStyleVar(2);
}

bool PropertyGrid::HasEdits() const noexcept
{
    return has_edits_;
}

void PropertyGrid::ClearEditFlag() noexcept
{
    has_edits_ = false;
}

void PropertyGrid::RenderField(const PropertyField& field,
                               const ellindyer::ui::fonts::FontSet& fonts,
                               std::size_t index)
{
    ImGui::PushID(static_cast<int>(index));

    switch (field.kind)
    {
    case PropertyFieldKind::Heading:
    {
        ImGui::Spacing();

        if (fonts.small_regular != nullptr)
        {
            ellindyer::ui::imgui_compat::PushFont(fonts.small_regular);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.68f, 0.68f, 0.68f, 1.00f));
        ImGui::TextUnformatted(field.label.c_str());
        ImGui::PopStyleColor();

        if (fonts.small_regular != nullptr)
        {
            ImGui::PopFont();
        }

        ImGui::Separator();
        break;
    }

    case PropertyFieldKind::Separator:
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        break;
    }

    case PropertyFieldKind::ReadOnlyText:
    {
        if (!field.label.empty())
        {
            if (fonts.small_regular != nullptr)
            {
                ellindyer::ui::imgui_compat::PushFont(fonts.small_regular);
            }
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.00f));
            ImGui::TextUnformatted(field.label.c_str());
            ImGui::PopStyleColor();
            if (fonts.small_regular != nullptr)
            {
                ImGui::PopFont();
            }
        }

        if (fonts.default_regular != nullptr)
        {
            ellindyer::ui::imgui_compat::PushFont(fonts.default_regular);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.90f, 0.90f, 1.00f));
        ImGui::TextWrapped("%s", field.text_value.c_str());
        ImGui::PopStyleColor();

        if (fonts.default_regular != nullptr)
        {
            ImGui::PopFont();
        }

        if (!field.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(field.tooltip.c_str());
            ImGui::EndTooltip();
        }
        break;
    }

    case PropertyFieldKind::Text:
    {
        char buffer[kTextBufferSize] = {};
        const std::size_t copy_length =
            field.text_value.size() < (kTextBufferSize - 1)
                ? field.text_value.size()
                : (kTextBufferSize - 1);
        std::memcpy(buffer, field.text_value.data(), copy_length);

        if (!field.label.empty())
        {
            ImGui::TextUnformatted(field.label.c_str());
        }

        ImGui::SetNextItemWidth(-1.0f);
        if (field.read_only)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::InputText("##Value", buffer, kTextBufferSize))
        {
            has_edits_ = true;
            if (field.on_text_changed)
            {
                field.on_text_changed(std::string(buffer));
            }
        }

        if (field.read_only)
        {
            ImGui::EndDisabled();
        }

        if (!field.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(field.tooltip.c_str());
            ImGui::EndTooltip();
        }
        break;
    }

    case PropertyFieldKind::MultilineText:
    {
        char buffer[kTextBufferSize] = {};
        const std::size_t copy_length =
            field.text_value.size() < (kTextBufferSize - 1)
                ? field.text_value.size()
                : (kTextBufferSize - 1);
        std::memcpy(buffer, field.text_value.data(), copy_length);

        if (!field.label.empty())
        {
            ImGui::TextUnformatted(field.label.c_str());
        }

        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::InputTextMultiline("##Value", buffer, kTextBufferSize,
                                      ImVec2(-1.0f, 70.0f)))
        {
            has_edits_ = true;
            if (field.on_text_changed)
            {
                field.on_text_changed(std::string(buffer));
            }
        }

        if (!field.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(field.tooltip.c_str());
            ImGui::EndTooltip();
        }
        break;
    }

    case PropertyFieldKind::Integer:
    {
        if (!field.label.empty())
        {
            ImGui::TextUnformatted(field.label.c_str());
        }

        int value = static_cast<int>(field.int_value);
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::InputInt("##Value", &value))
        {
            has_edits_ = true;
            if (field.on_int_changed)
            {
                field.on_int_changed(static_cast<std::int64_t>(value));
            }
        }

        if (!field.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(field.tooltip.c_str());
            ImGui::EndTooltip();
        }
        break;
    }

    case PropertyFieldKind::Float:
    {
        if (!field.label.empty())
        {
            ImGui::TextUnformatted(field.label.c_str());
        }

        float value = static_cast<float>(field.float_value);
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::InputFloat("##Value", &value, 0.0f, 0.0f, "%.6f"))
        {
            has_edits_ = true;
            if (field.on_float_changed)
            {
                field.on_float_changed(static_cast<double>(value));
            }
        }

        if (!field.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(field.tooltip.c_str());
            ImGui::EndTooltip();
        }
        break;
    }

    case PropertyFieldKind::Boolean:
    {
        bool value = field.bool_value;
        if (ImGui::Checkbox(field.label.c_str(), &value))
        {
            has_edits_ = true;
            if (field.on_bool_changed)
            {
                field.on_bool_changed(value);
            }
        }

        if (!field.tooltip.empty() && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(field.tooltip.c_str());
            ImGui::EndTooltip();
        }
        break;
    }
    }

    ImGui::PopID();
}

} // namespace ellindyer::ui::inspector
