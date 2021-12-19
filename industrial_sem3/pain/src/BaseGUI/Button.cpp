#include "Label.hpp"
#include "Button.hpp"

Button::Button(const IntRect &widgetRect, Widget *parent,
               const LGL::Color &idleColor, const LGL::Color &hoveredColor, const LGL::Color &pressedColor) : 
    AnimatedButton(widgetRect, parent),
    mIdleColor(idleColor), mHoveredColor(hoveredColor), mPressedColor(pressedColor)
{
}

void Button::onRedrawThis()
{
    if (mButtonPressed)
        mTexture.drawRect(IntRect(Vec2i(), mRect.size), mPressedColor);
    else
    {
        float animProgress = mCurrAnimTime / ANIM_TIME;
        LGL::Color currColor = mHoveredColor * animProgress + mIdleColor * (1 - animProgress);

        mTexture.drawRect(IntRect(Vec2i(), mRect.size), currColor);
    }
}

void Button::setLabel(const char *text)
{
    AnimatedButton::setLabel(text);
    // Center label
    mLabel->setPosition(Vec2i(mRect.size.x / 2 - mLabel->getRect().size.x / 2, mLabel->getRect().position.y));
}