#include "Button.hpp"
#include "Label.hpp"

Button::Button(const IntRect &widgetRect, Widget *parent,
               const LGL::Color &idleColor, const LGL::Color &hoveredColor, const LGL::Color &pressedColor) : 
    AnimatedButton(widgetRect, parent), mLabel(nullptr),
    mIdleColor(idleColor), mHoveredColor(hoveredColor), mPressedColor(pressedColor)
{
    mLabel = new Label(Vec2i(0, widgetRect.size.y / 2), this);
    addChild(mLabel);
}

void Button::setLabel(const char *label)
{
    mLabel->setText(label);
    // Center label
    mLabel->setPosition(Vec2i(mRect.size.x / 2 - mLabel->getRect().size.x / 2, mLabel->getRect().position.y));
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