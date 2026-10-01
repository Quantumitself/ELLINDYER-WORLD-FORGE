#include "UI/Graphics/DirectX11Context.hpp"

#include <d3d11.h>
#include <dxgi.h>

#if defined(_DEBUG)
#    include <d3d11sdklayers.h>
#endif

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace ellindyer::ui::graphics
{

namespace
{

constexpr DXGI_FORMAT kBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

} // namespace

DirectX11Context::DirectX11Context() = default;

DirectX11Context::~DirectX11Context()
{
    Destroy();
}

ellindyer::core::Result<void> DirectX11Context::Create(
    const DirectX11ContextDescription& description)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (device_ != nullptr)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "DirectX11Context is already initialized."));
    }

    if (description.window == nullptr)
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "DirectX11Context requires a valid HWND."));
    }

    width_  = description.width  == 0 ? 1U : description.width;
    height_ = description.height == 0 ? 1U : description.height;
    vsync_  = description.vsync;

    const Result<void> created = CreateDeviceAndSwapChain(description);
    if (created.HasError())
    {
        ReleaseAll();
        return created;
    }

    const Result<void> target = CreateRenderTarget();
    if (target.HasError())
    {
        ReleaseAll();
        return target;
    }

    return Result<void>{};
}

void DirectX11Context::Destroy()
{
    ReleaseAll();
    width_  = 0;
    height_ = 0;
    vsync_  = true;
}

bool DirectX11Context::IsValid() const noexcept
{
    return device_ != nullptr
        && device_context_ != nullptr
        && swap_chain_ != nullptr
        && render_target_view_ != nullptr;
}

ID3D11Device* DirectX11Context::GetDevice() const noexcept
{
    return device_;
}

ID3D11DeviceContext* DirectX11Context::GetDeviceContext() const noexcept
{
    return device_context_;
}

IDXGISwapChain* DirectX11Context::GetSwapChain() const noexcept
{
    return swap_chain_;
}

ID3D11RenderTargetView* DirectX11Context::GetRenderTargetView() const noexcept
{
    return render_target_view_;
}

ellindyer::core::Result<void> DirectX11Context::Resize(std::uint32_t width,
                                                       std::uint32_t height)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (width == 0 || height == 0)
    {
        return Result<void>{};
    }

    if (swap_chain_ == nullptr || device_ == nullptr)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "DirectX11Context is not initialized."));
    }

    if (width == width_ && height == height_)
    {
        return Result<void>{};
    }

    ReleaseRenderTarget();

    const HRESULT resize_result = swap_chain_->ResizeBuffers(
        0, width, height, DXGI_FORMAT_UNKNOWN, 0);

    if (FAILED(resize_result))
    {
        return Result<void>(MakeError(ErrorCode::PlatformError,
                                      "IDXGISwapChain::ResizeBuffers failed."));
    }

    width_  = width;
    height_ = height;

    return CreateRenderTarget();
}

void DirectX11Context::BeginFrame(float clear_r, float clear_g, float clear_b, float clear_a)
{
    if (!IsValid())
    {
        return;
    }

    const float clear_color[4] = {clear_r, clear_g, clear_b, clear_a};
    device_context_->OMSetRenderTargets(1, &render_target_view_, nullptr);
    device_context_->ClearRenderTargetView(render_target_view_, clear_color);
}

ellindyer::core::Result<void> DirectX11Context::Present()
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (swap_chain_ == nullptr)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "DirectX11Context has no swap chain."));
    }

    const UINT sync_interval = vsync_ ? 1U : 0U;
    const HRESULT result = swap_chain_->Present(sync_interval, 0);
    if (FAILED(result))
    {
        return Result<void>(MakeError(ErrorCode::PlatformError,
                                      "IDXGISwapChain::Present failed."));
    }
    return Result<void>{};
}

std::uint32_t DirectX11Context::GetWidth() const noexcept
{
    return width_;
}

std::uint32_t DirectX11Context::GetHeight() const noexcept
{
    return height_;
}

ellindyer::core::Result<void> DirectX11Context::CreateDeviceAndSwapChain(
    const DirectX11ContextDescription& description)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    UINT creation_flags = 0;
    if (description.enable_debug_layer)
    {
        creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
    }

    const D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    DXGI_SWAP_CHAIN_DESC swap_chain_description{};
    swap_chain_description.BufferCount        = 2;
    swap_chain_description.BufferDesc.Width   = width_;
    swap_chain_description.BufferDesc.Height  = height_;
    swap_chain_description.BufferDesc.Format  = kBackBufferFormat;
    swap_chain_description.BufferDesc.RefreshRate.Numerator   = 60;
    swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_description.BufferUsage                          = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_description.OutputWindow                         = description.window;
    swap_chain_description.SampleDesc.Count                     = 1;
    swap_chain_description.SampleDesc.Quality                   = 0;
    swap_chain_description.Windowed                            = TRUE;
    swap_chain_description.SwapEffect                          = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_description.Flags                               = 0;

    D3D_FEATURE_LEVEL achieved_level = D3D_FEATURE_LEVEL_11_0;

    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creation_flags,
        feature_levels,
        static_cast<UINT>(sizeof(feature_levels) / sizeof(feature_levels[0])),
        D3D11_SDK_VERSION,
        &swap_chain_description,
        &swap_chain_,
        &device_,
        &achieved_level,
        &device_context_);

    if (FAILED(result) && description.enable_debug_layer)
    {
        creation_flags &= ~static_cast<UINT>(D3D11_CREATE_DEVICE_DEBUG);

        result = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creation_flags,
            feature_levels,
            static_cast<UINT>(sizeof(feature_levels) / sizeof(feature_levels[0])),
            D3D11_SDK_VERSION,
            &swap_chain_description,
            &swap_chain_,
            &device_,
            &achieved_level,
            &device_context_);
    }

    if (FAILED(result))
    {
        return Result<void>(MakeError(ErrorCode::PlatformError,
                                      "D3D11CreateDeviceAndSwapChain failed."));
    }

    return Result<void>{};
}

ellindyer::core::Result<void> DirectX11Context::CreateRenderTarget()
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    ID3D11Texture2D* back_buffer = nullptr;
    HRESULT result = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                            reinterpret_cast<void**>(&back_buffer));
    if (FAILED(result) || back_buffer == nullptr)
    {
        return Result<void>(MakeError(ErrorCode::PlatformError,
                                      "IDXGISwapChain::GetBuffer failed."));
    }

    result = device_->CreateRenderTargetView(back_buffer, nullptr, &render_target_view_);
    back_buffer->Release();

    if (FAILED(result) || render_target_view_ == nullptr)
    {
        return Result<void>(MakeError(ErrorCode::PlatformError,
                                      "ID3D11Device::CreateRenderTargetView failed."));
    }

    return Result<void>{};
}

void DirectX11Context::ReleaseRenderTarget()
{
    if (render_target_view_ != nullptr)
    {
        render_target_view_->Release();
        render_target_view_ = nullptr;
    }
}

void DirectX11Context::ReleaseAll()
{
    ReleaseRenderTarget();

    if (swap_chain_ != nullptr)
    {
        swap_chain_->Release();
        swap_chain_ = nullptr;
    }

    if (device_context_ != nullptr)
    {
        device_context_->Release();
        device_context_ = nullptr;
    }

    if (device_ != nullptr)
    {
        device_->Release();
        device_ = nullptr;
    }
}

} // namespace ellindyer::ui::graphics
