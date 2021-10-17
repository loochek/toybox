#include "WindowHeader.hpp"
#include "Window.hpp"
#include "Label.hpp"
#include "Button.hpp"

static const int HEADER_HEIGHT = 20;

WindowHeader::WindowHeader(Window *parent) :
    Widget(IntRect(Vec2i(), Vec2i(parent->getRect().size.x, HEADER_HEIGHT)), parent)
{
    Label *windowLabel = new Label(this);
    windowLabel->setText("Test window");
    addChild(windowLabel);

    Button *button = new Button(IntRect(Vec2i(mRect.size.x - HEADER_HEIGHT, 0),
                                        Vec2i(HEADER_HEIGHT, HEADER_HEIGHT)),
                                this,
                                LGL::Color::Red);

    button->setLabel("X");
    addChild(button);
}

void WindowHeader::redrawThis()
{
    mTexture.drawRect(FloatRect(Vec2f(), mRect.size), LGL::Color::Cyan);
}