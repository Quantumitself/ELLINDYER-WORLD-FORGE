#include "UI/Dialogs/DialogBase.hpp"

#include <utility>

namespace ellindyer::ui::dialogs
{

DialogBase::DialogBase()
    : identifier_("##WorldForgeDialog")
{
}

DialogBase::DialogBase(std::string identifier, std::string title)
    : identifier_(std::move(identifier))
    , title_(std::move(title))
{
    if (identifier_.empty())
    {
        identifier_ = "##WorldForgeDialog";
    }
}

DialogBase::~DialogBase() = default;

void DialogBase::SetIdentifier(std::string identifier)
{
    if (identifier.empty())
    {
        return;
    }
    identifier_ = std::move(identifier);
}

const std::string& DialogBase::GetIdentifier() const noexcept
{
    return identifier_;
}

void DialogBase::SetTitle(std::string title)
{
    title_ = std::move(title);
}

const std::string& DialogBase::GetTitle() const noexcept
{
    return title_;
}

void DialogBase::SetSubtitle(std::string subtitle)
{
    subtitle_ = std::move(subtitle);
}

const std::string& DialogBase::GetSubtitle() const noexcept
{
    return subtitle_;
}

void DialogBase::SetStyle(const DialogBaseStyle& style)
{
    style_ = style;
}

const DialogBaseStyle& DialogBase::GetStyle() const noexcept
{
    return style_;
}

void DialogBase::Open()
{
    if (is_open_)
    {
        return;
    }
    is_open_ = true;
    popup_opened_ = false;
    ClearErrorMessage();
    OnOpened();
}

void DialogBase::Close()
{
    if (!is_open_)
    {
        return;
    }
    is_open_ = false;
    popup_opened_ = false;
    ImGui::CloseCurrentPopup();
    OnClosed();
}

void DialogBase::SetErrorMessage(std::string message)
{
    error_message_ = std::move(message);
}

void DialogBase::ClearErrorMessage()
{
    error_message_.clear();
}

bool DialogBase::IsOpen() const noexcept
{
    return is_open_;
}

bool DialogBase::HasErrorMessage() const noexcept
{
    return !error_message_.empty();
}

const std::string& DialogBase::GetErrorMessage() const noexcept
{
    return error_message_;
}

DialogResult DialogBase::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    DialogResult result{};
    if (!is_open_)
    {
        return result;
    }

    const ImGuiIO& io = ImGui::GetIO();

    const ImVec2 window_size(style_.width, style_.height);
    const ImVec2 window_position = ComputeCenteredPosition(io.DisplaySize);



    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2(style_.padding_x, style_.padding_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, style_.background_color);
    ImGui::PushStyleColor(ImGuiCol_Border,   style_.border_color);

    // Open the popup exactly once when the dialog first becomes visible.
    if (!popup_opened_)
    {
        popup_opened_ = true;
        ImGui::SetNextWindowPos(window_position, ImGuiCond_Always);
        ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
        ImGui::OpenPopup(identifier_.c_str());
    }

    bool accepted = false;
    bool cancelled = false;

    if (ImGui::BeginPopupModal(identifier_.c_str(), nullptr, flags))
    {
        RenderTitle(fonts);

        if (HasErrorMessage())
        {
            RenderError(fonts);
        }

        ImGui::BeginChild("##DialogBody",
                          ImVec2(0.0f, GetBodyHeight()),
                          false,
                          ImGuiWindowFlags_NoScrollbar);

        RenderBody(fonts);

        ImGui::EndChild();

        RenderFooterButtons(fonts, accepted, cancelled);

        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);

    if (accepted)
    {
        OnAccept();
        Close();
        result.outcome = DialogOutcome::Accepted;
        return result;
    }

    if (cancelled)
    {
        OnCancel();
        Close();
        result.outcome = DialogOutcome::Cancelled;
        return result;
    }

    result.outcome = DialogOutcome::Open;
    return result;
}

void DialogBase::RenderTitle(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (fonts.large_regular != nullptr)
    {
        ImGui::PushFont(fonts.large_regular, fonts.large_regular->LegacySize);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, style_.title_color);
    ImGui::TextUnformatted(title_.c_str());
    ImGui::PopStyleColor();

    if (fonts.large_regular != nullptr)
    {
        ImGui::PopFont();
    }

    if (!subtitle_.empty())
    {
        if (fonts.small_regular != nullptr)
        {
            ImGui::PushFont(fonts.small_regular, fonts.small_regular->LegacySize);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, style_.subtitle_color);
        ImGui::TextUnformatted(subtitle_.c_str());
        ImGui::PopStyleColor();

        if (fonts.small_regular != nullptr)
        {
            ImGui::PopFont();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void DialogBase::RenderError(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, style_.error_color);
    ImGui::TextWrapped("%s", error_message_.c_str());
    ImGui::PopStyleColor();

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }

    ImGui::Spacing();
}

void DialogBase::RenderFooterButtons(const ellindyer::ui::fonts::FontSet& fonts,
                                     bool& out_accept,
                                     bool& out_cancel)
{
    const float avail_width = ImGui::GetContentRegionAvail().x;
    const float total_button_width =
        style_.button_width * 2.0f + ImGui::GetStyle().ItemSpacing.x;
    const float start_x = ImGui::GetCursorPosX() + avail_width - total_button_width;

    if (start_x > ImGui::GetCursorPosX())
    {
        ImGui::SetCursorPosX(start_x);
    }

    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    const bool accept_enabled = CanAccept();
    const bool cancel_enabled = CanCancel();

    if (!accept_enabled)
    {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button(GetAcceptLabel(), ImVec2(style_.button_width, style_.button_height)))
    {
        out_accept = true;
    }
    if (!accept_enabled)
    {
        ImGui::EndDisabled();
    }

    ImGui::SameLine();

    if (!cancel_enabled)
    {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button(GetCancelLabel(), ImVec2(style_.button_width, style_.button_height)))
    {
        out_cancel = true;
    }
    if (!cancel_enabled)
    {
        ImGui::EndDisabled();
    }

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
    {
        out_cancel = true;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Enter, false) && accept_enabled)
    {
        out_accept = true;
    }
}

ImVec2 DialogBase::ComputeCenteredPosition(const ImVec2& display_size) const noexcept
{
    ImVec2 position(0.0f, 0.0f);
    if (style_.center_horizontally)
    {
        position.x = (display_size.x - style_.width) * 0.5f;
    }
    if (style_.center_vertically)
    {
        position.y = (display_size.y - style_.height) * 0.5f;
    }
    return position;
}

float DialogBase::GetBodyHeight() const noexcept
{
    const float reserved =
        style_.padding_y * 2.0f
        + style_.footer_height
        + ImGui::GetFrameHeightWithSpacing() * 2.0f;

    float height = style_.height - reserved;
    if (height < 0.0f)
    {
        height = 0.0f;
    }
    return height;
}

} // namespace ellindyer::ui::dialogs
