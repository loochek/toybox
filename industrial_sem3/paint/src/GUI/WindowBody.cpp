#include "WindowBody.hpp"

WindowBody::WindowBody(const IntRect &widgetRect, Widget *parent) : Widget(widgetRect, parent)
{
}

void WindowBody::redrawThis()
{
    mTexture.drawRect(FloatRect(Vec2f(), mRect.size));
    //printf("body: size: %d %d\n", mRect.size.x, mRect.size.y);
}