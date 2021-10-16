#include "SFMLRenderTexture.hpp"

namespace LGL
{
    RenderTexture::RenderTexture(const Vec2i &resolution) : RenderTarget(&mTexture)
    {
        mTexture.create(resolution.x, resolution.y);
    }
};