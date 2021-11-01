#include "SFMLRenderTexture.hpp"

namespace LGL
{
    RenderTexture::RenderTexture(const Vec2i &resolution) : RenderTarget(&mTexture)
    {
        mTexture.create(resolution.x, resolution.y);
    }

    void RenderTexture::resize(const Vec2i &newResolution)
    {
        mTexture.create(newResolution.x, newResolution.y);
    }
};