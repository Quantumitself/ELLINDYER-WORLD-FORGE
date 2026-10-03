#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <imgui.h>

#include "UI/Fonts/FontManager.hpp"

namespace ellindyer::ui::dialogs
{

struct RecentProjectsListStyle
{
    float   row_height           = 22.0f;
    float   padding_x            = 8.0f;
    float   padding_y            = 6.0f;
    ImVec4  selected_color       = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    ImVec4  hovered_color        = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    ImVec4  primary_text         = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    ImVec4  secondary_text       = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
    ImVec4  empty_text           = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
};

class RecentProjectsList
{
public:
    RecentProjectsList();
    ~RecentProjectsList();

    RecentProjectsList(const RecentProjectsList&) = delete;
    RecentProjectsList& operator=(const RecentProjectsList&) = delete;
    RecentProjectsList(RecentProjectsList&&) noexcept = delete;
    RecentProjectsList& operator=(RecentProjectsList&&) noexcept = delete;

    void SetStyle(const RecentProjectsListStyle& style);

    [[nodiscard]] const RecentProjectsListStyle& GetStyle() const noexcept;

    void SetEntries(std::vector<std::filesystem::path> entries);

    void Clear();

    [[nodiscard]] const std::vector<std::filesystem::path>& GetEntries() const noexcept;

    [[nodiscard]] std::size_t GetEntryCount() const noexcept;

    [[nodiscard]] bool HasEntries() const noexcept;

    void SetSelectedIndex(int index) noexcept;

    [[nodiscard]] int GetSelectedIndex() const noexcept;

    [[nodiscard]] bool HasSelection() const noexcept;

    [[nodiscard]] std::filesystem::path GetSelectedPath() const;

    [[nodiscard]] bool Render(const ellindyer::ui::fonts::FontSet& fonts,
                              float height,
                              bool* out_double_click = nullptr);

private:
    RecentProjectsListStyle           style_{};
    std::vector<std::filesystem::path> entries_;
    int                                selected_index_ = -1;
};

} // namespace ellindyer::ui::dialogs
