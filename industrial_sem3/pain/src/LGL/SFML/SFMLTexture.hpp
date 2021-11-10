#ifndef LGL_SFML_TEXTURE_HPP
#define LGL_SFML_TEXTURE_HPP

#include <SFML/Graphics.hpp>
#include "../Color.hpp"
#include "../../Utils/Vec2.hpp"

namespace LGL
{
    class Image;
    class RenderTexture;

    /**
     * Represent texture in the GPU memory
     */
    class Texture
    {
    public:
        /**
         * Loads texture from file
         * 
         * \param filename File name
         */
        bool loadFromFile(const char *filename);

        /**
         * Loads texture from image
         * 
         * \param image Image
         */
        bool loadFromImage(const Image &image);
        
        /**
         * Loads texture from the render texture
         * 
         * \param renderTexture Render texture
         */
        void loadFromRenderTexture(const RenderTexture &renderTexture);

        /**
         * Makes an image from the texture
         * 
         * \return Image
         */
        Image copyToImage() const;

        /**
         * \return Texture size
         */
        Vec2i getSize() const;

    private:
        // to be able to draw mTexture and create texture from render texture
        friend class RenderTarget;
        sf::Texture mTexture;
    };
}

#endif