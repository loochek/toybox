#include "Button.hpp"

Button::Button(const IntRect &widgetRect, Widget *parent,
               const LGL::Color &idleColor, const LGL::Color &hoveredColor, const LGL::Color &pressedColor) : 
    AnimatedButton(widgetRect, parent), mLabel(nullptr),
    mIdleColor(idleColor), mHoveredColor(hoveredColor), mPressedColor(pressedColor)
{
    mLabel = new Label(this);
    addChild(mLabel);
}

void Button::setLabel(const char *label)
{
    mLabel->setText(label);

    Vec2f newLabelPos = mRect.size / 2 - mLabel->getRect().size / 2;
    mLabel->setPosition(newLabelPos);
}

void Button::onRedrawThis()
{
    if (mButtonPressed)
        mTexture.drawRect(FloatRect(Vec2i(), mRect.size), mPressedColor);
    else
    {
        float animProgress = mCurrAnimTime / ANIM_TIME;
        LGL::Color currColor = mHoveredColor * animProgress + mIdleColor * (1 - animProgress);

        mTexture.drawRect(FloatRect(Vec2i(), mRect.size), currColor);
    }
}