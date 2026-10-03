#pragma once

#include <cstdint>
#include <string>

#include <imgui.h>

#include "UI/Dialogs/DialogResult.hpp"
#include "UI/Fonts/FontManager.hpp"

namespace ellindyer::ui::dialogs
{

struct DialogBaseStyle
{
    float   width              = 520.0f;
    float   height             = 360.0f;
    float   padding_x          = 16.0f;
    float   padding_y          = 14.0f;
    float   footer_height      = 34.0f;
    float   button_width       = 96.0f;
    float   button_height      = 26.0f;
    ImVec4  background_color   = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    ImVec4  border_color       = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    ImVec4  title_color        = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    ImVec4  subtitle_color     = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
    ImVec4  error_color        = ImVec4(0.92f, 0.45f, 0.45f, 1.00f);
    ImVec4  dim_background     = ImVec4(0.00f, 0.00f, 0.00f, 0.45f);
    bool    center_horizontally = true;
    bool    center_vertically   = true;
};

class DialogBase
{
public:
    DialogBase();
    explicit DialogBase(std::string identifier, std::string title);
    virtual ~DialogBase();

    DialogBase(const DialogBase&) = delete;
    DialogBase& operator=(const DialogBase&) = delete;
    DialogBase(DialogBase&&) noexcept = delete;
    DialogBase& operator=(DialogBase&&) noexcept = delete;

    void SetIdentifier(std::string identifier);

    [[nodiscard]] const std::string& GetIdentifier() const noexcept;

    void SetTitle(std::string title);

    [[nodiscard]] const std::string& GetTitle() const noexcept;

    void SetSubtitle(std::string subtitle);

    [[nodiscard]] const std::string& GetSubtitle() const noexcept;

    void SetStyle(const DialogBaseStyle& style);

    [[nodiscard]] const DialogBaseStyle& GetStyle() const noexcept;

    void Open();

    void Close();

    void SetErrorMessage(std::string message);

    void ClearErrorMessage();

    [[nodiscard]] bool IsOpen() const noexcept;

    [[nodiscard]] bool HasErrorMessage() const noexcept;

    [[nodiscard]] const std::string& GetErrorMessage() const noexcept;

    [[nodiscard]] DialogResult Render(const ellindyer::ui::fonts::FontSet& fonts);

protected:
    virtual void OnOpened() {}

    virtual void OnClosed() {}

    virtual void RenderBody(const ellindyer::ui::fonts::FontSet& fonts) = 0;

    [[nodiscard]] virtual bool CanAccept() const = 0;

    [[nodiscard]] virtual bool CanCancel() const { return true; }

    virtual void OnAccept() {}

    virtual void OnCancel() {}

    [[nodiscard]] virtual const char* GetAcceptLabel() const { return "OK"; }

    [[nodiscard]] virtual const char* GetCancelLabel() const { return "Cancel"; }

    void RenderTitle(const ellindyer::ui::fonts::FontSet& fonts);

    void RenderError(const ellindyer::ui::fonts::FontSet& fonts);

    void RenderFooterButtons(const ellindyer::ui::fonts::FontSet& fonts,
                             bool& out_accept,
                             bool& out_cancel);

    [[nodiscard]] ImVec2 ComputeCenteredPosition(const ImVec2& display_size) const noexcept;

    [[nodiscard]] float GetBodyHeight() const noexcept;

private:
    std::string     identifier_;
    std::string     title_;
    std::string     subtitle_;
    std::string     error_message_;
    DialogBaseStyle style_{};
    bool            is_open_ = false;
    bool            popup_opened_ = false;
};

} // namespace ellindyer::ui::dialogs
