#include <cstdio>
#include <cstring>
#include "TextureManager.hpp"

const char* TEXTURE_INDEX  = "resources/textures.idx";
const char* TEXTURE_PREFIX = "resources/textures/";

const int MAX_STR_LEN = 256;

TextureManager::~TextureManager()
{
    for (TextureEntry entry : mTextures)
        delete entry.texture;
}

void TextureManager::load()
{
    FILE *index = fopen(TEXTURE_INDEX, "r");
    if (index == nullptr)
        throw std::runtime_error("Unable to open texture index");

    char textureId[MAX_STR_LEN + 1] = {0};
    char fileName [MAX_STR_LEN + 1] = {0};
    
    char buf[MAX_STR_LEN + 1] = {0};

    while (fgets(buf, MAX_STR_LEN, index) != NULL)
    {
        sscanf(buf, "%256s %256s", textureId, fileName);

        char pathBuf[MAX_STR_LEN * 2 + 1] = {0};
        
        strcpy(pathBuf, TEXTURE_PREFIX);
        strcat(pathBuf, fileName);
        
        TextureEntry entry = {0};
        
        entry.texture = new LGL::Texture();
        if (!entry.texture->loadFromFile(pathBuf))
        {
            printf("[WARNING] Unable to load texture %s\n", textureId);
            delete entry.texture;
            continue;
        }

        strncpy(entry.identifier, textureId, MAX_ID_LEN);
        mTextures.push_back(entry);
    }

    if (!feof(index))
    {
        fclose(index);
        throw std::runtime_error("Unable to read texture index");
    }

    fclose(index);
}

const LGL::Texture *TextureManager::getTexture(const char *identifier)
{
    for (TextureEntry entry : mTextures)
    {
        if (strncmp(entry.identifier, identifier, MAX_ID_LEN) == 0)
            return entry.texture;
    }

    return nullptr;
}

TextureManager *TextureManager::getInstance()
{
    static TextureManager mgr;
    return &mgr;
}