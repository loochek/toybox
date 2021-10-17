#include <stdexcept>
#include "Widget.hpp"

Widget::Widget(const IntRect &widgetRect, Widget *parent) : mRect(widgetRect), mTexture(widgetRect.size) {};

Widget::~Widget()
{
    for (Widget *child : mChildren)
        delete child;
}

void Widget::onUpdate(const Vec2i &parentAbsPos, float elapsedTime)
{
    Vec2i currAbsPos = parentAbsPos + mRect.position;
    for (Widget *child : mChildren)
        child->onUpdate(currAbsPos, elapsedTime);
}

void Widget::onRedraw()
{
    mTexture.clear();
    redrawThis();
    for (Widget *child : mChildren)
    {
        child->onRedraw();
        mTexture.drawRenderTexture(child->mTexture, child->mRect.position);
    }
}

bool Widget::addChild(Widget *child)
{
    if (!IntRect(Vec2i(), mRect.size).contains(child->mRect))
        return false;

    mChildren.push_back(child);
    return true;
}