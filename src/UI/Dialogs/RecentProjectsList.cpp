#include "UI/Dialogs/RecentProjectsList.hpp"

#include <utility>

namespace ellindyer::ui::dialogs
{

namespace
{

constexpr const char* kEmptyLabel = "No recent projects.";

} // namespace

RecentProjectsList::RecentProjectsList() = default;

RecentProjectsList::~RecentProjectsList() = default;

void RecentProjectsList::SetStyle(const RecentProjectsListStyle& style)
{
    style_ = style;
}

const RecentProjectsListStyle& RecentProjectsList::GetStyle() const noexcept
{
    return style_;
}

void RecentProjectsList::SetEntries(std::vector<std::filesystem::path> entries)
{
    entries_ = std::move(entries);
    selected_index_ = entries_.empty() ? -1 : 0;
}

void RecentProjectsList::Clear()
{
    entries_.clear();
    selected_index_ = -1;
}

const std::vector<std::filesystem::path>& RecentProjectsList::GetEntries() const noexcept
{
    return entries_;
}

std::size_t RecentProjectsList::GetEntryCount() const noexcept
{
    return entries_.size();
}

bool RecentProjectsList::HasEntries() const noexcept
{
    return !entries_.empty();
}

void RecentProjectsList::SetSelectedIndex(int index) noexcept
{
    if (index < -1)
    {
        index = -1;
    }
    if (index >= static_cast<int>(entries_.size()))
    {
        index = entries_.empty() ? -1 : static_cast<int>(entries_.size()) - 1;
    }
    selected_index_ = index;
}

int RecentProjectsList::GetSelectedIndex() const noexcept
{
    return selected_index_;
}

bool RecentProjectsList::HasSelection() const noexcept
{
    return selected_index_ >= 0
        && selected_index_ < static_cast<int>(entries_.size());
}

std::filesystem::path RecentProjectsList::GetSelectedPath() const
{
    if (!HasSelection())
    {
        return {};
    }
    return entries_[static_cast<std::size_t>(selected_index_)];
}

bool RecentProjectsList::Render(const ellindyer::ui::fonts::FontSet& fonts,
                                float height,
                                bool* out_double_click)
{
    bool selection_changed = false;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2(style_.padding_x, style_.padding_y));

    ImGui::BeginChild("##RecentProjectsListChild",
                      ImVec2(0.0f, height),
                      true,
                      ImGuiWindowFlags_NoScrollbar
                          | ImGuiWindowFlags_NoScrollWithMouse);

    if (entries_.empty())
    {
        if (fonts.default_regular != nullptr)
        {
            ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, style_.empty_text);
        ImGui::TextUnformatted(kEmptyLabel);
        ImGui::PopStyleColor();
        if (fonts.default_regular != nullptr)
        {
            ImGui::PopFont();
        }
    }
    else
    {
        const float row_height = style_.row_height;
        const float start_y = ImGui::GetCursorPosY();

        for (std::size_t index = 0; index < entries_.size(); ++index)
        {
            const std::filesystem::path& entry = entries_[index];
            const bool is_selected =
                (static_cast<int>(index) == selected_index_);

            const std::string label = entry.filename().string().empty()
                ? entry.generic_string()
                : entry.filename().string();

            const std::string secondary = entry.parent_path().generic_string();

            const float row_top = start_y
                + static_cast<float>(index) * (row_height + 2.0f);
            const float cursor_y = row_top;

            ImGui::SetCursorPosY(cursor_y);

            ImGui::PushID(static_cast<int>(index));

            const ImVec2 row_start = ImGui::GetCursorScreenPos();
            const float row_width = ImGui::GetContentRegionAvail().x;

            const bool is_hovered =
                ImGui::IsMouseHoveringRect(row_start,
                                           ImVec2(row_start.x + row_width,
                                                  row_start.y + row_height));

            if (is_selected || is_hovered)
            {
                ImGui::GetWindowDrawList()->AddRectFilled(
                    row_start,
                    ImVec2(row_start.x + row_width, row_start.y + row_height),
                    ImGui::GetColorU32(is_selected ? style_.selected_color
                                                    : style_.hovered_color));
            }

            ImGui::SetCursorScreenPos(ImVec2(row_start.x + 6.0f,
                                             row_start.y + 2.0f));

            if (fonts.default_regular != nullptr)
            {
                ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
            }

            ImGui::PushStyleColor(ImGuiCol_Text, style_.primary_text);
            ImGui::TextUnformatted(label.c_str());
            ImGui::PopStyleColor();

            if (fonts.default_regular != nullptr)
            {
                ImGui::PopFont();
            }

            if (!secondary.empty())
            {
                ImGui::SameLine();

                if (fonts.small_regular != nullptr)
                {
                    ImGui::PushFont(fonts.small_regular, fonts.small_regular->LegacySize);
                }

                ImGui::PushStyleColor(ImGuiCol_Text, style_.secondary_text);
                ImGui::TextUnformatted(secondary.c_str());
                ImGui::PopStyleColor();

                if (fonts.small_regular != nullptr)
                {
                    ImGui::PopFont();
                }
            }

            ImGui::InvisibleButton("##RecentRowButton",
                                   ImVec2(row_width, row_height));

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                if (selected_index_ != static_cast<int>(index))
                {
                    selected_index_ = static_cast<int>(index);
                    selection_changed = true;
                }
            }

            if (out_double_click != nullptr && ImGui::IsItemHovered()
                && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                *out_double_click = true;
                selected_index_ = static_cast<int>(index);
                selection_changed = true;
            }

            ImGui::PopID();
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();

    return selection_changed;
}

} // namespace ellindyer::ui::dialogs
