#ifndef TEXTURE_BUTTON_HPP
#define TEXTURE_BUTTON_HPP

#include "../LGL/LGL.hpp"
#include "AnimatedButton.hpp"

/**
 * Button with custom textures
 */
class TextureButton : public AnimatedButton
{
public:
    TextureButton() = delete;
    TextureButton(const IntRect &widgetRect,
                  const LGL::Texture *idleTexture,
                  const LGL::Texture *hoveredTexture,
                  const LGL::Texture *pressedTexture,
                  Widget *parent = nullptr);

    virtual ~TextureButton() {};

    /**
     * Sets idle texture
     * 
     * \param texture Texture
     */
    void setIdleTexture(const LGL::Texture *texture) { mIdleTexture = texture; };

    /**
     * Sets hovered texture
     * 
     * \param texture Texture
     */
    void setHoveredTexture(const LGL::Texture *texture) { mHoveredTexture = texture; };

    /**
     * Sets pressed texture
     * 
     * \param texture Texture
     */
    void setPressedTexture(const LGL::Texture *texture) { mPressedTexture = texture; };

protected:
    virtual void onRedrawThis() override;

protected:
    const LGL::Texture *mIdleTexture;
    const LGL::Texture *mHoveredTexture;
    const LGL::Texture *mPressedTexture;
};

#endif