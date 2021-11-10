#ifndef LGL_SFML_IMAGE_HPP
#define LGL_SFML_IMAGE_HPP

#include <SFML/Graphics.hpp>
#include "../Color.hpp"
#include "../../Utils/Vec2.hpp"

namespace LGL
{
    /**
     * Represent image
     */
    class Image
    {
    public:
        /**
         * Creates texture with blank color
         * 
         * \param width Texture width
         * \param height Texture height
         * \param color Fill color
         */
        void create(int width, int height, const Color &color = Color::Transparent);

        /**
         * Loads image from file
         * 
         * \param filename File name
         */
        bool loadFromFile(const char *filename);

        /**
         * Saves image to file
         * 
         * \param filename File name
         */
        bool saveToFile(const char *filename) const;

        /**
         * Sets pixel of the image
         * 
         * \param x Pixel X
         * \param y Pixel Y
         * \param color Pixel color
         */
        void setPixel(int x, int y, const Color &color);

        /**
         * \return Image size
         */
        Vec2i getSize() const;

    private:
        // to access mImage
        friend class Texture;
        sf::Image mImage;
    };
}

#endif