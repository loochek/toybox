#include "WindowHeader.hpp"
#include "Window.hpp"
#include "Button.hpp"

const int WindowHeader::HEADER_HEIGHT = 20;

WindowHeader::WindowHeader(Window *parent) :
    Widget(IntRect(Vec2i(), Vec2i(parent->getRect().size.x, HEADER_HEIGHT)), parent),
    mOldMousePosition(Vec2i()), mMousePressed(false)
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

void WindowHeader::onRedrawThis()
{
    mTexture.drawRect(FloatRect(Vec2f(), mRect.size), LGL::Color::Cyan);
}

void WindowHeader::onMouseMoveThis(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    // Using global mouse position because of self-moving

    if (mMousePressed)
        mParent->move(globalMousePos - mOldMousePosition);
    
    mOldMousePosition = globalMousePos;
}

void WindowHeader::onMouseClickedThis()
{
    mMousePressed = true;
}

void WindowHeader::onMouseReleasedThis()
{
    mMousePressed = false;
}