#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <vector>
#include "LGL/LGL.hpp"

const int MAX_ID_LEN = 256;

/**
 * Class responsible for loading and storing textures
 */
class TextureManager
{
public:
    /**
     * Gives a required texture or null pointer if there is no texture with such identifier
     * 
     * \param identifier Texture identifier
     * \return Texture or null pointer
     */
    const LGL::Texture *getTexture(const char *identifier);

    /**
     * Loads textures into the manager
     */
    void load();

    /**
     * \return Texture manager singleton
     */
    static TextureManager *getInstance();

private:
    TextureManager() {};
    ~TextureManager();

private:
    struct TextureEntry
    {
        char identifier[MAX_ID_LEN];
        LGL::Texture* texture;
    };

    std::vector<TextureEntry> mTextures;
};

#endif