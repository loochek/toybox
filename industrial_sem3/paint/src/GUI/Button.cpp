#include "Button.hpp"
#include "../Graphics.hpp"

Button::Button(const IntRect &rect, Widget *parent,
               const Color &idleColor, const Color &hoveredColor, const Color &pressedColor) : 
               Widget(rect, parent), mDelegate(nullptr), mLabel(nullptr), mCurrColor(idleColor),
               mIdleColor(idleColor), mHoveredColor(hoveredColor), mPressedColor(pressedColor)
{
}

Button::~Button()
{
    if (mDelegate != nullptr)
        delete mDelegate;
}

void Button::draw(Graphics &graphics)
{
    graphics.drawRect(mRect, mCurrColor);
    if (mLabel != nullptr)
        graphics.drawText(mRect.position + mRect.size / 2, mLabel, TextOrigin::Centered);
}

// void Button::onMouseHoverBegin()
// {
//     currColor = hoveredColor;
// }

// void Button::onMouseClicked()
// {
//     currColor = pressedColor;
//     if (delegate != nullptr)
//         delegate->onClick(this);
// }

// void Button::onMouseReleased()
// {
//     currColor = hoveredColor;
// }

// void Button::onMouseHoverEnd()
// {
//     currColor = idleColor;
// }