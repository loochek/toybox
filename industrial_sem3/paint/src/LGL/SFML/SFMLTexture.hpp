#ifndef LGL_SFML_TEXTURE_HPP
#define LGL_SFML_TEXTURE_HPP

#include <SFML/Graphics.hpp>

namespace LGL
{
    class Texture
    {
    public:
        /**
         * Loads texture from file
         * 
         * \param filename File name
         */
        bool loadFromFile(const char *filename);

    private:
        // to be able to draw mTexture
        friend class RenderTarget;
        sf::Texture mTexture;
    };
}

#endif