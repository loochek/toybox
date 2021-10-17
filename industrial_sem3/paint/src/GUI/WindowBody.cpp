#include "WindowBody.hpp"

WindowBody::WindowBody(const IntRect &widgetRect, Widget *parent) : Widget(widgetRect, parent)
{
}

void WindowBody::redrawThis()
{
    mTexture.drawRect(FloatRect(Vec2f(), mRect.size));
}