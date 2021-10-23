#include "BaseButton.hpp"

BaseButton::BaseButton(const IntRect &widgetRect, Widget *parent) :
    Widget(widgetRect, parent), mDelegate(nullptr)
{
}

BaseButton::~BaseButton()
{
    if (mDelegate != nullptr)
        delete mDelegate;
}