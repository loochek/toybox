#include "TextureButton.hpp"

TextureButton::TextureButton(const IntRect &widgetRect,
                             const LGL::Texture *idleTexture,
                             const LGL::Texture *hoveredTexture,
                             const LGL::Texture *pressedTexture,
                             Widget *parent) : 
    AnimatedButton(widgetRect, parent),
    mIdleTexture(idleTexture), mHoveredTexture(hoveredTexture), mPressedTexture(pressedTexture)
{
}

void TextureButton::onRedrawThis()
{
    if (mButtonPressed)
        mTexture.drawTexture(*mPressedTexture, Vec2i());
    else if (mButtonHovered)
        mTexture.drawTexture(*mHoveredTexture, Vec2i());
    else
        mTexture.drawTexture(*mIdleTexture, Vec2i());
}