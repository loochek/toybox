#include "Window.hpp"
#include "Label.hpp"
#include "Button.hpp"
#include "DragArea.hpp"
#include "../GUILogic/WindowCloseDelegate.hpp"

const int Window::HEADER_HEIGHT      = 30;
const int Window::SIDE_BORDER_SIZE   = 8;
const int Window::BOTTOM_BORDER_SIZE = 8;

const int CLOSE_BUTTON_WIDTH  = 20;
const int CLOSE_BUTTON_HEIGHT = 20;
const int CLOSE_BUTTON_OFFSET = 10;

const int LABEL_HEIGHT_OFFSET = 3;

Window::Window(const IntRect &contentRect, Widget *parent) :
    Widget(IntRect(contentRect.position - Vec2i(SIDE_BORDER_SIZE, HEADER_HEIGHT),
                   contentRect.size + Vec2i(SIDE_BORDER_SIZE * 2, HEADER_HEIGHT + BOTTOM_BORDER_SIZE)),
           parent),
    mTitle(nullptr)
{
    DragArea *dragArea = new DragArea(IntRect(Vec2i(), Vec2i(mRect.size.x, HEADER_HEIGHT)), this, this);
    addChild(dragArea);

    Button *button = new Button(IntRect(Vec2i(mRect.size.x - CLOSE_BUTTON_OFFSET - CLOSE_BUTTON_WIDTH, 0),
                                        Vec2i(CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT)),
                                this,
                                LGL::Color::Red);

    button->setLabel("X");
    button->setDelegate(new WindowCloseDelegate(this));
    addChild(button);
}

void Window::setTitle(const char *title)
{
    if (mTitle == nullptr)
    {
        mTitle = new Label(this);
        mTitle->setPosition(Vec2i(SIDE_BORDER_SIZE, LABEL_HEIGHT_OFFSET));
        addChild(mTitle);
    }

    mTitle->setText(title);
}

void Window::onRedrawThis()
{
    mTexture.drawRect(IntRect(Vec2i(), mRect.size), LGL::Color(0.2f, 0.2f, 0.2f));
}