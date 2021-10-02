#include "Button.hpp"
#include "Graphics.hpp"

Button::Button(const Rect &rect,
               const Color &idleColor, const Color &selectedColor, const Color &pressedColor) : 
               RectangularWidget(rect), delegate(nullptr),
               idleColor(idleColor), selectedColor(selectedColor), pressedColor(pressedColor)
{
}

void Button::update()
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
    
    Vec2f p1 = rect.position;
    Vec2f p2 = rect.position + Vec2f(rect.size.x, 0);
    Vec2f p3 = rect.position + rect.size;
    Vec2f p4 = rect.position + Vec2f(0, rect.size.y);
    
    graphics.drawQuad(p1, p2, p3, p4, color);
}

void Button::setDelegate(ButtonDelegate *delegate)
{
    this->delegate = delegate;
}