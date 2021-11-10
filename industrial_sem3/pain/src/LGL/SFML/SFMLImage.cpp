#include "SFMLImage.hpp"
#include "../LGL.hpp"

namespace LGL
{
    void Image::create(int width, int height, const Color &color)
    {
        mImage.create(width, height, toSFMLColor(color));
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

    Vec2i Image::getSize() const
    {
        return fromSFMLVector(mImage.getSize());
    }
}