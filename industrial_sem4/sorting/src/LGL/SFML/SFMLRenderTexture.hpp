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

        /**
         * Resizes the render texture
         * 
         * \param newResolution New resolution
         */
        void resize(const Vec2i &newResolution);

        /** 
         * \return newResolution Render texture resolution
         */
        Vec2i getSize() const;

    private:
        // to be able to draw mTexture
        friend class RenderTarget;
        sf::RenderTexture mTexture;

        // to be able to create Texture from RenderTexture
        friend class Texture;
    };
};

#endif