#include "UI/Branding/LogoTexture.hpp"

#include "UI/Textures/TextureLoader.hpp"

namespace ellindyer::ui::branding
{

LogoTexture::LogoTexture() = default;

LogoTexture::~LogoTexture()
{
    Release();
}

ellindyer::core::Result<void> LogoTexture::Load(
    ellindyer::ui::graphics::DirectX11Context& graphics,
    const BrandingAssets& assets)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (loaded_)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "LogoTexture is already loaded."));
    }

    if (assets.logo_path.empty())
    {
        return Result<void>(MakeError(ErrorCode::ResourceMissing,
                                      "Application logo asset is not present."));
    }

    Result<ellindyer::ui::textures::TextureHandle> texture_result =
        ellindyer::ui::textures::TextureLoader::LoadFromFile(graphics, assets.logo_path);

    if (texture_result.HasError())
    {
        return Result<void>(texture_result.GetError());
    }

    texture_ = texture_result.Value();
    loaded_  = true;
    return Result<void>{};
}

void LogoTexture::Release()
{
    if (loaded_)
    {
        ellindyer::ui::textures::TextureLoader::Release(texture_);
        loaded_ = false;
    }
}

bool LogoTexture::IsLoaded() const noexcept
{
    return loaded_ && texture_.IsValid();
}

const ellindyer::ui::textures::TextureHandle& LogoTexture::GetTexture() const noexcept
{
    return texture_;
}

} // namespace ellindyer::ui::branding
