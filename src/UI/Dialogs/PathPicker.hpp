#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include <imgui.h>

#include "UI/Fonts/FontManager.hpp"

namespace ellindyer::ui::dialogs
{

struct PathPickerStyle
{
    float   input_width        = 0.0f;
    float   browse_button_width = 78.0f;
    float   input_height       = 24.0f;
    ImVec4  hint_color         = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
};

class PathPicker
{
public:
    PathPicker();
    ~PathPicker();

    PathPicker(const PathPicker&) = delete;
    PathPicker& operator=(const PathPicker&) = delete;
    PathPicker(PathPicker&&) noexcept = delete;
    PathPicker& operator=(PathPicker&&) noexcept = delete;

    void SetLabel(std::string label);

    [[nodiscard]] const std::string& GetLabel() const noexcept;

    void SetHint(std::string hint);

    [[nodiscard]] const std::string& GetHint() const noexcept;

    void SetStyle(const PathPickerStyle& style);

    [[nodiscard]] const PathPickerStyle& GetStyle() const noexcept;

    void SetValue(std::filesystem::path value);

    [[nodiscard]] const std::filesystem::path& GetValue() const noexcept;

    void SetReadOnly(bool read_only) noexcept;

    [[nodiscard]] bool IsReadOnly() const noexcept;

    [[nodiscard]] bool Render(const ellindyer::ui::fonts::FontSet& fonts);

private:
    std::string         label_;
    std::string         hint_;
    PathPickerStyle     style_{};
    std::filesystem::path value_;
    bool                read_only_ = false;
    char                buffer_[1024] = {};
};

} // namespace ellindyer::ui::dialogs
