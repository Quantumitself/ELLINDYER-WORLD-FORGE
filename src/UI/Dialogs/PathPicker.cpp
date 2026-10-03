#include "UI/Dialogs/PathPicker.hpp"

#include <algorithm>
#include <cstring>
#include <utility>

namespace ellindyer::ui::dialogs
{

namespace
{

constexpr std::size_t kBufferSize = sizeof(PathPicker::buffer_);

void CopyToBuffer(char* buffer, std::size_t buffer_size, const std::string& text)
{
    if (buffer_size == 0)
    {
        return;
    }
    const std::size_t copy_length = std::min(buffer_size - 1, text.size());
    std::memcpy(buffer, text.data(), copy_length);
    buffer[copy_length] = '\0';
}

} // namespace

PathPicker::PathPicker()
{
    buffer_[0] = '\0';
}

PathPicker::~PathPicker() = default;

void PathPicker::SetLabel(std::string label)
{
    label_ = std::move(label);
}

const std::string& PathPicker::GetLabel() const noexcept
{
    return label_;
}

void PathPicker::SetHint(std::string hint)
{
    hint_ = std::move(hint);
}

const std::string& PathPicker::GetHint() const noexcept
{
    return hint_;
}

void PathPicker::SetStyle(const PathPickerStyle& style)
{
    style_ = style;
}

const PathPickerStyle& PathPicker::GetStyle() const noexcept
{
    return style_;
}

void PathPicker::SetValue(std::filesystem::path value)
{
    value_ = std::move(value);
    CopyToBuffer(buffer_, kBufferSize, value_.generic_string());
}

const std::filesystem::path& PathPicker::GetValue() const noexcept
{
    return value_;
}

void PathPicker::SetReadOnly(bool read_only) noexcept
{
    read_only_ = read_only;
}

bool PathPicker::IsReadOnly() const noexcept
{
    return read_only_;
}

bool PathPicker::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    if (!label_.empty())
    {
        ImGui::TextUnformatted(label_.c_str());
    }

    const float available_width = ImGui::GetContentRegionAvail().x;
    const float input_width =
        (style_.input_width > 0.0f)
            ? style_.input_width
            : (available_width - style_.browse_button_width - ImGui::GetStyle().ItemSpacing.x);

    ImGui::SetNextItemWidth(input_width);

    if (read_only_)
    {
        ImGui::BeginDisabled();
    }

    if (ImGui::InputText("##PathPickerValue", buffer_, kBufferSize))
    {
        value_ = std::filesystem::path(buffer_);
    }

    if (read_only_)
    {
        ImGui::EndDisabled();
    }

    ImGui::SameLine();

    const bool browse_pressed =
        ImGui::Button("Browse", ImVec2(style_.browse_button_width, style_.input_height));

    if (!hint_.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, style_.hint_color);
        ImGui::TextWrapped("%s", hint_.c_str());
        ImGui::PopStyleColor();
    }

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }

    return browse_pressed;
}

} // namespace ellindyer::ui::dialogs
