 #include "SFMLTexture.hpp"

namespace LGL
{
    bool Texture::loadFromFile(const char *filename)
    {
        return mTexture.loadFromFile(filename);
    }
}