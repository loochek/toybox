#include "../LGL/LGL.hpp"
#include "ButtonBar.hpp"
#include "Button.hpp"

const int ButtonBar::MENU_BAR_HEIGHT = 22;

const int BUTTON_SPACING = 15;

ButtonBar::ButtonBar(Widget *parent, ButtonBarLocation location) :
    Widget(IntRect(Vec2i(), Vec2i(parent->getRect().size.x, MENU_BAR_HEIGHT)), parent), mFilledWidth(0)
{
    if (location == ButtonBarLocation::Bottom)
        setPosition(Vec2i(0, parent->getRect().size.y - MENU_BAR_HEIGHT));
}

const BaseButton *ButtonBar::addButton(const char *text, ButtonDelegate *delegate, int userData)
{
    Vec2i textBounds = LGL::RenderTarget::calculateTextBounds(text);

    int buttonWidth = textBounds.x + BUTTON_SPACING * 2;
    Button *button = new Button(IntRect(Vec2i(mFilledWidth, 0), Vec2i(buttonWidth, MENU_BAR_HEIGHT)),
                                this, LGL::Color::White);
    button->setLabel(text);
    button->setDelegate(delegate);
    button->setUserData(userData);
    addChild(button);

    mFilledWidth += buttonWidth;
    return button;
}

void ButtonBar::deleteButton(const BaseButton *button)
{
    mChildren.remove((Widget*)button);
    rearrangeButtons();
}

void ButtonBar::onRedrawThis()
{
    mTexture.drawRect(mRect, LGL::Color::White);
}

void ButtonBar::rearrangeButtons()
{
    mFilledWidth = 0;

    for (Widget *button : mChildren)
    {
        button->setPosition(Vec2i(mFilledWidth, 0));
        mFilledWidth += button->getRect().size.x;
    }
}