#include "WindowHeader.hpp"
#include "Window.hpp"
#include "Button.hpp"

const int WindowHeader::HEADER_HEIGHT = 20;

WindowHeader::WindowHeader(Window *parent) :
    Widget(IntRect(Vec2i(), Vec2i(parent->getRect().size.x, HEADER_HEIGHT)), parent)
{
    mTitle = new Label(this);
    addChild(mTitle);

    Button *button = new Button(IntRect(Vec2i(mRect.size.x - HEADER_HEIGHT, 0),
                                        Vec2i(HEADER_HEIGHT, HEADER_HEIGHT)),
                                this,
                                LGL::Color::Red);

    button->setLabel("X");
    addChild(button);
}

void WindowHeader::setTitle(const char *title)
{
    mTitle->setText(title);
}

void WindowHeader::redrawThis()
{
    mTexture.drawRect(FloatRect(Vec2f(), mRect.size), LGL::Color::Cyan);
}