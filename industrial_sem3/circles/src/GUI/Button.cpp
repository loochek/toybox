#include "Button.hpp"
#include "../Graphics.hpp"

Button::Button(const Rect &rect,
               const Color &idleColor, const Color &hoveredColor, const Color &pressedColor) : 
               RectangularWidget(rect), delegate(nullptr), label(nullptr), currColor(idleColor),
               idleColor(idleColor), hoveredColor(hoveredColor), pressedColor(pressedColor)
{
}

Button::~Button()
{
    if (delegate != nullptr)
        delete delegate;
}

void Button::draw(Graphics &graphics)
{
    graphics.drawRect(rect, currColor);
    if (label != nullptr)
        graphics.drawText(rect.position + rect.size / 2, label, TextOrigin::Centered);
}

void Button::onMouseHoverBegin()
{
    currColor = hoveredColor;
}

void Button::onMouseClicked()
{
    currColor = pressedColor;
    if (delegate != nullptr)
        delegate->onClick(this);
}

void Button::onMouseReleased()
{
    currColor = hoveredColor;
}

void Button::onMouseHoverEnd()
{
    currColor = idleColor;
}