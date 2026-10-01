#pragma once

#include <imgui.h>

namespace ellindyer::ui::imgui_compat
{

inline void PushFont(ImFont* font)
{
    ImGui::PushFont(font, font != nullptr ? font->LegacySize : 0.0f);
}

inline void PopFont()
{
    ImGui::PopFont();
}

} // namespace ellindyer::ui::imgui_compat
