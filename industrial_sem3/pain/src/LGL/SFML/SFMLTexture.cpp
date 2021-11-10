#include "SFMLTexture.hpp"
#include "../LGL.hpp"

namespace LGL
{
    bool Texture::loadFromFile(const char *filename)
    {
        return mTexture.loadFromFile(filename);
    }

    bool Texture::loadFromImage(const Image &image)
    {
        return mTexture.loadFromImage(image.mImage);
    }

    void Texture::loadFromRenderTexture(const RenderTexture &renderTexture)
    {
        mTexture = sf::Texture(renderTexture.mTexture.getTexture());
    }

    Image Texture::copyToImage() const
    {
        Image image;
        image.mImage = mTexture.copyToImage();
        
        return image;
    }

    Vec2i Texture::getSize() const
    {
        return fromSFMLVector(mTexture.getSize());
    }
}