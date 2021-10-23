#include "TextureButton.hpp"

TextureButton::TextureButton(const IntRect &widgetRect,
                             const LGL::Texture *idleTexture,
                             const LGL::Texture *hoveredTexture,
                             const LGL::Texture *pressedTexture,
                             Widget *parent) : 
    BaseButton(widgetRect, parent), mCurrTexture(idleTexture),
    mIdleTexture(idleTexture), mHoveredTexture(hoveredTexture), mPressedTexture(pressedTexture)
{
}

void TextureButton::onRedrawThis()
{
    mTexture.drawTexture(*mCurrTexture, Vec2i());
}

void TextureButton::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mCurrTexture = mHoveredTexture;
}

void TextureButton::onMouseClicked()
{
    mCurrTexture = mPressedTexture;
    if (mDelegate != nullptr)
        mDelegate->operator()();
}

void TextureButton::onMouseReleased()
{
    mCurrTexture = mHoveredTexture;
}

void TextureButton::onMouseHoverEnd()
{
    mCurrTexture = mIdleTexture;
}