#include "MenuBar.hpp"
#include "../LGL/LGL.hpp"
#include "Button.hpp"

const int MenuBar::MENU_BAR_HEIGHT = 20;

const int BUTTON_SPACING = 10;

MenuBar::MenuBar(Widget *parent) :
    Widget(IntRect(Vec2i(), Vec2i(parent->getRect().size.x, MENU_BAR_HEIGHT)), parent),
    mFilledWidth(0)
{
}

void MenuBar::addButton(const char *text, ButtonDelegate *delegate, int userData)
{
    Vec2i textBounds = LGL::RenderTarget::calculateTextBounds(text);

    int buttonWidth = textBounds.x + BUTTON_SPACING * 2;
    Button *button = new Button(IntRect(Vec2i(mFilledWidth, 0), Vec2i(buttonWidth, MENU_BAR_HEIGHT)),
                                this, LGL::Color::Transparent);
    button->setLabel(text);
    button->setDelegate(delegate);
    button->setUserData(userData);
    addChild(button);

    mFilledWidth += buttonWidth;
}

void MenuBar::onRedrawThis()
{
    mTexture.drawRect(mRect, LGL::Color::White);
}