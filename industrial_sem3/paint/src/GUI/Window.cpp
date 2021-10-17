#include "Window.hpp"
#include "WindowHeader.hpp"
#include "WindowBody.hpp"

Window::Window(const IntRect &windowRect, Widget *parent) : Widget(windowRect, parent)
{
    WindowHeader *header = new WindowHeader(this);
    int headerHeight = header->getRect().size.y;

    WindowBody *body = new WindowBody(IntRect(Vec2i(0, headerHeight), windowRect.size - Vec2i(0, headerHeight)),
                                      this);

    addChild(header);
    addChild(body);
}

void Window::redrawThis()
{
    mTexture.drawRect(FloatRect(Vec2i(), mRect.size), LGL::Color::Magenta);
}