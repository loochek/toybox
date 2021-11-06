#ifndef LGL_SFML_TEXTURE_HPP
#define LGL_SFML_TEXTURE_HPP

#include <SFML/Graphics.hpp>
#include "../Color.hpp"
#include "../../Utils/Vec2.hpp"

namespace LGL
{
    class Texture
    {
    public:
        /**
         * Creates texture with blank color
         * 
         * \param width Texture width
         * \param height Texture height
         * \param color Fill color
         */
        bool create(int width, int height, const Color &color = Color::Transparent);

        /**
         * Loads texture from file
         * 
         * \param filename File name
         */
        bool loadFromFile(const char *filename);

        /**
         * Sets pixel of the texture image
         * Applied only after update()
         * 
         * \param x Pixel X
         * \param y Pixel Y
         * \param color Pixel color
         */
        void setPixel(int x, int y, const Color &color);
        
        /**
         * Updates texture with internal image
         */
        void update();

        /**
         * \return Texture size
         */
        Vec2i getSize() const;

    private:
        // to be able to draw mTexture
        friend class RenderTarget;
        sf::Texture mTexture;
        sf::Image   mImage;
    };
}

#endif