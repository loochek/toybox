#include "SFMLImage.hpp"
#include "../LGL.hpp"

namespace LGL
{
    void Image::create(int width, int height, const Color &color)
    {
        mImage.create(width, height, toSFMLColor(color));
    }

    void Image::create(int width, int height, const uint8_t *pixels)
    {
        mImage.create(width, height, pixels);
    }

    bool Image::loadFromFile(const char *filename)
    {
        return mImage.loadFromFile(filename);
    }

    bool Image::saveToFile(const char *filename) const
    {
        return mImage.saveToFile(filename);
    }

    void Image::setPixel(int x, int y, const Color &color)
    {
        mImage.setPixel(x, y, toSFMLColor(color));
    }

    const uint8_t *Image::getPixels()
    {
        return mImage.getPixelsPtr();
    }

    Vec2i Image::getSize() const
    {
        return fromSFMLVector(mImage.getSize());
    }
}