#pragma once

#include <windows.h>

namespace ellindyer::ui::imgui_layer
{

class ImGuiWin32MessageHook
{
public:
    ImGuiWin32MessageHook() = delete;

    [[nodiscard]] static bool HandleMessage(HWND window,
                                            UINT message,
                                            WPARAM wparam,
                                            LPARAM lparam);
};

} // namespace ellindyer::ui::imgui_layer
