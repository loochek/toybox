#include "Button.hpp"

Button::Button(const IntRect &widgetRect, Widget *parent,
               const LGL::Color &idleColor, const LGL::Color &hoveredColor, const LGL::Color &pressedColor) : 
    BaseButton(widgetRect, parent), mLabel(nullptr), 
    mCurrColor(idleColor), mIdleColor(idleColor), mHoveredColor(hoveredColor), mPressedColor(pressedColor)
{
}

void Button::onRedrawThis()
{
    mTexture.drawRect(FloatRect(Vec2i(), mRect.size), mCurrColor);
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

void Button::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mCurrColor = mHoveredColor;
}

void Button::onMouseClicked()
{
    mCurrColor = mPressedColor;
    if (mDelegate != nullptr)
        mDelegate->operator()();
}

void Button::onMouseReleased()
{
    mCurrColor = mHoveredColor;
}

void Button::onMouseHoverEnd()
{
    mCurrColor = mIdleColor;
}