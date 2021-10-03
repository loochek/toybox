#include "Button.hpp"
#include "../Graphics.hpp"

Button::Button(const Rect &rect,
               const Color &idleColor, const Color &selectedColor, const Color &pressedColor) : 
               RectangularWidget(rect), delegate(nullptr), label(nullptr),
               idleColor(idleColor), selectedColor(selectedColor), pressedColor(pressedColor)
{
}

void Button::update(float elapsedTime)
{
    if (state == WidgetState::Clicked && delegate != nullptr)
        delegate->onClick(this);
}

void Button::draw(Graphics &graphics)
{
    Color color;

    switch (state)
    {
    case WidgetState::Idle:
        color = idleColor;
        break;

    case WidgetState::Selected:
    case WidgetState::Released:
        color = selectedColor;
        break;

    case WidgetState::Clicked:
    case WidgetState::Pressed:
        color = pressedColor;
        break;
    }
    
    graphics.drawRect(rect, color);
    if (label != nullptr)
        graphics.drawText(rect.position + rect.size / 2, label, TextOrigin::Centered);
}

void Button::setDelegate(ButtonDelegate *delegate)
{
    this->delegate = delegate;
}

void Button::setLabel(const char *label)
{
    this->label = label;
}