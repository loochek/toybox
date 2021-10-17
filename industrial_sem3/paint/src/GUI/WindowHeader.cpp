#include "WindowHeader.hpp"
#include "Window.hpp"

static const int HEADER_HEIGHT = 20;

WindowHeader::WindowHeader(Window *parent) :
    Widget(IntRect(Vec2i(), Vec2i(parent->getRect().size.x, HEADER_HEIGHT)), parent)
{
}

void WindowHeader::redrawThis()
{
    mTexture.drawRect(FloatRect(Vec2f(), mRect.size), LGL::Color::Cyan);
    //printf("header: size: %d %d\n", mRect.size.x, mRect.size.y);
}