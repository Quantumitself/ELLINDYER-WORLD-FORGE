#include "UI/Textures/TextureLoader.hpp"

#include <d3d11.h>

#include <stb/stb_image.h>

namespace ellindyer::ui::textures
{

ellindyer::core::Result<TextureHandle> TextureLoader::LoadFromFile(
    ellindyer::ui::graphics::DirectX11Context& graphics,
    const std::filesystem::path& path)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (!graphics.IsValid())
    {
        return Result<TextureHandle>(MakeError(ErrorCode::InvalidState,
                                               "DirectX11Context is not valid."));
    }

    if (path.empty())
    {
        return Result<TextureHandle>(MakeError(ErrorCode::InvalidArgument,
                                               "Texture path is empty."));
    }

    const std::string path_text = path.generic_string();

    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_set_flip_vertically_on_load(0);

    unsigned char* pixels = stbi_load(path_text.c_str(), &width, &height, &channels, 4);
    if (pixels == nullptr)
    {
        return Result<TextureHandle>(MakeError(ErrorCode::ResourceLoadFailed,
                                               "stbi_load failed for: " + path_text));
    }

    if (width <= 0 || height <= 0)
    {
        stbi_image_free(pixels);
        return Result<TextureHandle>(MakeError(ErrorCode::ResourceCorrupt,
                                               "Decoded image has invalid dimensions."));
    }

    D3D11_TEXTURE2D_DESC texture_description{};
    texture_description.Width            = static_cast<UINT>(width);
    texture_description.Height           = static_cast<UINT>(height);
    texture_description.MipLevels        = 1;
    texture_description.ArraySize        = 1;
    texture_description.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_description.SampleDesc.Count = 1;
    texture_description.SampleDesc.Quality = 0;
    texture_description.Usage            = D3D11_USAGE_DEFAULT;
    texture_description.BindFlags        = D3D11_BIND_SHADER_RESOURCE;
    texture_description.CPUAccessFlags   = 0;
    texture_description.MiscFlags        = 0;

    D3D11_SUBRESOURCE_DATA initial_data{};
    initial_data.pSysMem          = pixels;
    initial_data.SysMemPitch      = static_cast<UINT>(width) * 4U;
    initial_data.SysMemSlicePitch = 0;

    ID3D11Texture2D* texture = nullptr;
    HRESULT result = graphics.GetDevice()->CreateTexture2D(
        &texture_description, &initial_data, &texture);

    stbi_image_free(pixels);

    if (FAILED(result) || texture == nullptr)
    {
        return Result<TextureHandle>(MakeError(ErrorCode::PlatformError,
                                               "CreateTexture2D failed."));
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC view_description{};
    view_description.Format                    = texture_description.Format;
    view_description.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    view_description.Texture2D.MostDetailedMip = 0;
    view_description.Texture2D.MipLevels       = 1;

    ID3D11ShaderResourceView* view = nullptr;
    result = graphics.GetDevice()->CreateShaderResourceView(texture, &view_description, &view);
    if (FAILED(result) || view == nullptr)
    {
        texture->Release();
        return Result<TextureHandle>(MakeError(ErrorCode::PlatformError,
                                               "CreateShaderResourceView failed."));
    }

    TextureHandle handle{};
    handle.view    = view;
    handle.texture = texture;
    handle.width   = static_cast<std::uint32_t>(width);
    handle.height  = static_cast<std::uint32_t>(height);

    return Result<TextureHandle>(handle);
}

void TextureLoader::Release(TextureHandle& handle)
{
    if (handle.view != nullptr)
    {
        handle.view->Release();
        handle.view = nullptr;
    }
    if (handle.texture != nullptr)
    {
        handle.texture->Release();
        handle.texture = nullptr;
    }
    handle.width  = 0;
    handle.height = 0;
}

} // namespace ellindyer::ui::textures
