#include "UI/Platform/PlatformWindow.hpp"

#include <atomic>
#include <string>

namespace ellindyer::ui::platform
{

namespace
{

constexpr const wchar_t* kWindowClassNamePrefix = L"EllindyerWorldForgeWindowClass";

std::atomic<std::uint64_t> g_window_class_counter{0};

std::wstring GenerateWindowClassName()
{
    const std::uint64_t value = g_window_class_counter.fetch_add(1, std::memory_order_relaxed);
    return std::wstring(kWindowClassNamePrefix) + L"_" + std::to_wstring(value);
}

} // namespace

PlatformWindow::PlatformWindow() = default;

PlatformWindow::~PlatformWindow()
{
    Destroy();
}

ellindyer::core::Result<void> PlatformWindow::Create(
    const PlatformWindowDescription& description)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (window_handle_ != nullptr)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "PlatformWindow is already created."));
    }

    class_name_ = RegisterWindowClass();
    if (class_name_.empty())
    {
        return Result<void>(MakeError(ErrorCode::PlatformError,
                                      "Failed to register window class."));
    }

    width_      = description.width;
    height_     = description.height;
    min_width_  = description.min_width;
    min_height_ = description.min_height;
    resizable_  = description.resizable;

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    if (resizable_)
    {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    RECT window_rect{};
    window_rect.left   = 0;
    window_rect.top    = 0;
    window_rect.right  = static_cast<LONG>(width_);
    window_rect.bottom = static_cast<LONG>(height_);

    AdjustWindowRectEx(&window_rect, style, FALSE, 0);

    const int final_width  = window_rect.right - window_rect.left;
    const int final_height = window_rect.bottom - window_rect.top;

    int position_x = CW_USEDEFAULT;
    int position_y = CW_USEDEFAULT;

    if (description.centered)
    {
        const int screen_width  = GetSystemMetrics(SM_CXSCREEN);
        const int screen_height = GetSystemMetrics(SM_CYSCREEN);
        position_x = (screen_width  - final_width)  / 2;
        position_y = (screen_height - final_height) / 2;
    }

    window_handle_ = CreateWindowExW(
        0,
        class_name_.c_str(),
        description.title.c_str(),
        style,
        position_x,
        position_y,
        final_width,
        final_height,
        nullptr,
        nullptr,
        GetModuleHandleW(nullptr),
        this);

    if (window_handle_ == nullptr)
    {
        return Result<void>(MakeError(ErrorCode::PlatformError,
                                      "CreateWindowExW failed."));
    }

    should_close_ = false;
    return Result<void>{};
}

void PlatformWindow::Destroy()
{
    if (window_handle_ != nullptr)
    {
        DestroyWindow(window_handle_);
        window_handle_ = nullptr;
    }

    if (!class_name_.empty())
    {
        UnregisterClassW(class_name_.c_str(), GetModuleHandleW(nullptr));
        class_name_.clear();
    }

    should_close_ = false;
    resize_callback_ = nullptr;
    close_callback_  = nullptr;
}

bool PlatformWindow::IsValid() const noexcept
{
    return window_handle_ != nullptr && IsWindow(window_handle_) != FALSE;
}

HWND PlatformWindow::GetHandle() const noexcept
{
    return window_handle_;
}

std::uint32_t PlatformWindow::GetWidth() const noexcept
{
    return width_;
}

std::uint32_t PlatformWindow::GetHeight() const noexcept
{
    return height_;
}

void PlatformWindow::Show()
{
    if (window_handle_ != nullptr)
    {
        ShowWindow(window_handle_, SW_SHOW);
        UpdateWindow(window_handle_);
    }
}

void PlatformWindow::Hide()
{
    if (window_handle_ != nullptr)
    {
        ShowWindow(window_handle_, SW_HIDE);
    }
}

void PlatformWindow::SetTitle(std::wstring_view title)
{
    if (window_handle_ != nullptr)
    {
        SetWindowTextW(window_handle_, std::wstring(title).c_str());
    }
}

void PlatformWindow::SetResizeCallback(ResizeCallback callback)
{
    resize_callback_ = std::move(callback);
}

void PlatformWindow::SetCloseCallback(CloseCallback callback)
{
    close_callback_ = std::move(callback);
}

bool PlatformWindow::PumpMessages()
{
    MSG message{};
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
    {
        if (message.message == WM_QUIT)
        {
            should_close_ = true;
            return false;
        }
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return true;
}

void PlatformWindow::RequestClose()
{
    should_close_ = true;
}

bool PlatformWindow::ShouldClose() const noexcept
{
    return should_close_;
}

LRESULT CALLBACK PlatformWindow::WindowProcedureThunk(HWND window,
                                                     UINT message,
                                                     WPARAM wparam,
                                                     LPARAM lparam)
{
    if (message == WM_NCCREATE)
    {
        const auto* create_struct = reinterpret_cast<CREATESTRUCTW*>(lparam);
        auto* self = static_cast<PlatformWindow*>(create_struct->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        return DefWindowProcW(window, message, wparam, lparam);
    }

    auto* self = reinterpret_cast<PlatformWindow*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (self != nullptr)
    {
        return self->WindowProcedure(window, message, wparam, lparam);
    }

    return DefWindowProcW(window, message, wparam, lparam);
}

LRESULT PlatformWindow::WindowProcedure(HWND window,
                                        UINT message,
                                        WPARAM wparam,
                                        LPARAM lparam)
{
    switch (message)
    {
    case WM_SIZE:
    {
        if (wparam != SIZE_MINIMIZED)
        {
            const std::uint32_t new_width  = static_cast<std::uint32_t>(LOWORD(lparam));
            const std::uint32_t new_height = static_cast<std::uint32_t>(HIWORD(lparam));
            if (new_width != width_ || new_height != height_)
            {
                width_  = new_width;
                height_ = new_height;
                if (resize_callback_)
                {
                    resize_callback_(width_, height_);
                }
            }
        }
        return 0;
    }

    case WM_GETMINMAXINFO:
    {
        auto* info = reinterpret_cast<MINMAXINFO*>(lparam);
        info->ptMinTrackSize.x = static_cast<LONG>(min_width_);
        info->ptMinTrackSize.y = static_cast<LONG>(min_height_);
        return 0;
    }

    case WM_CLOSE:
    {
        should_close_ = true;
        if (close_callback_)
        {
            close_callback_();
        }
        DestroyWindow(window);
        return 0;
    }

    case WM_DESTROY:
    {
        should_close_ = true;
        window_handle_ = nullptr;
        PostQuitMessage(0);
        return 0;
    }

    case WM_ERASEBKGND:
    {
        return 1;
    }

    case WM_SYSCOMMAND:
    {
        if ((wparam & 0xFFF0) == SC_KEYMENU)
        {
            return 0;
        }
        break;
    }

    default:
        break;
    }

    return DefWindowProcW(window, message, wparam, lparam);
}

std::wstring PlatformWindow::RegisterWindowClass()
{
    const std::wstring class_name = GenerateWindowClassName();

    WNDCLASSEXW window_class{};
    window_class.cbSize        = sizeof(WNDCLASSEXW);
    window_class.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc   = &PlatformWindow::WindowProcedureThunk;
    window_class.cbClsExtra    = 0;
    window_class.cbWndExtra    = 0;
    window_class.hInstance     = GetModuleHandleW(nullptr);
    window_class.hIcon         = LoadIconW(nullptr, IDI_APPLICATION);
    window_class.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    window_class.hbrBackground = nullptr;
    window_class.lpszMenuName  = nullptr;
    window_class.lpszClassName = class_name.c_str();
    window_class.hIconSm       = LoadIconW(nullptr, IDI_APPLICATION);

    if (RegisterClassExW(&window_class) == 0)
    {
        return std::wstring{};
    }

    return class_name;
}

} // namespace ellindyer::ui::platform
