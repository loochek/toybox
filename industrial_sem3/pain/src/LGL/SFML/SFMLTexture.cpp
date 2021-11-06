#include "SFMLTexture.hpp"
#include "../LGL.hpp"

namespace LGL
{
    bool Texture::create(int width, int height, const Color &color)
    {
        mImage.create(width, height, toSFMLColor(color));
        
        return mTexture.loadFromImage(mImage);
    }

    bool Texture::loadFromFile(const char *filename)
    {
        bool result = mImage.loadFromFile(filename);
        if (!result)
            return false;

        return mTexture.loadFromImage(mImage);
    }

    void Texture::setPixel(int x, int y, const Color &color)
    {
        mImage.setPixel(x, y, toSFMLColor(color));
    }

    Vec2i Texture::getSize() const
    {
        return fromSFMLVector(mTexture.getSize());
    }

    void Texture::update()
    {
        mTexture.update(mImage);
    }
}