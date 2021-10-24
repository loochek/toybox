#include "Button.hpp"

const float ANIM_TIME = 0.4f;

Button::Button(const IntRect &widgetRect, Widget *parent,
               const LGL::Color &idleColor, const LGL::Color &hoveredColor, const LGL::Color &pressedColor) : 
    BaseButton(widgetRect, parent), mLabel(nullptr),
    mButtonHovered(false), mButtonPressed(false), mCurrAnimTime(0.0f),
    mIdleColor(idleColor), mHoveredColor(hoveredColor), mPressedColor(pressedColor)
{
}

void Button::setLabel(const char *label)
{
    if (mLabel == nullptr)
    {
        mLabel = new Label(this);
        addChild(mLabel);
    }
    
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

void Button::onUpdateThis(float elapsedTime)
{
    if (mButtonHovered)
    {
        if (mCurrAnimTime < ANIM_TIME)
            mCurrAnimTime = std::min(mCurrAnimTime + elapsedTime, ANIM_TIME);
    }
    else
    {
        if (mCurrAnimTime > 0.0f)
            mCurrAnimTime = std::max(mCurrAnimTime - elapsedTime, 0.0f);
    }
}

void Button::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mButtonHovered = true;
}

void Button::onMouseClicked()
{
    mButtonPressed = true;
    mCurrAnimTime = ANIM_TIME;

    if (mDelegate != nullptr)
        mDelegate->onClick(mUserData);
}

void Button::onMouseReleased()
{
    mButtonPressed = false;
}

void Button::onMouseHoverEnd()
{
    mButtonHovered = false;
}