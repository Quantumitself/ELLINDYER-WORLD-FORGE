#pragma once

#include <imgui.h>

namespace ellindyer::ui::imgui_compat
{

inline void PushFont(ImFont* font)
{
    if (font == nullptr)
    {
        return;
    }
    ImGui::PushFont(font, font->LegacySize);
}

} // namespace ellindyer::ui::imgui_compat
