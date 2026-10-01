#pragma once

#include <string>

#include "UI/Fonts/FontManager.hpp"

namespace ellindyer::ui::panels
{

class StatusBarPanel
{
public:
    StatusBarPanel();
    ~StatusBarPanel();

    StatusBarPanel(const StatusBarPanel&) = delete;
    StatusBarPanel& operator=(const StatusBarPanel&) = delete;
    StatusBarPanel(StatusBarPanel&&) noexcept = delete;
    StatusBarPanel& operator=(StatusBarPanel&&) noexcept = delete;

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

    void SetLeftText(std::string text);
    void SetMiddleText(std::string text);
    void SetRightText(std::string text);

private:
    std::string left_text_   = "Ready";
    std::string middle_text_;
    std::string right_text_;
};

} // namespace ellindyer::ui::panels
