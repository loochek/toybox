#ifndef LGL_SFML_RENDER_TEXTURE_HPP
#define LGL_SFML_RENDER_TEXTURE_HPP

#include "SFMLRenderTarget.hpp"

namespace LGL
{
    /**
     * A render target for off-screen rendering
     */
    class RenderTexture : public RenderTarget
    {
    public:
        /**
         * \param resolution Render texture resolution
         */
        RenderTexture(const Vec2i &resolution);

    private:
        // to be able to draw mTexture
        friend class RenderTarget;
        sf::RenderTexture mTexture;
    };
};

#endif