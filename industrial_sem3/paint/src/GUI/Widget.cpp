#include <stdexcept>
#include "Widget.hpp"

Widget::Widget(const IntRect &widgetRect, Widget *parent) : mRect(widgetRect) {};

void Widget::update(float elapsedTime)
{
    updateChildren(elapsedTime);
}

void Widget::updateChildren(float elapsedTime)
{
    for (Widget *child : mChildren)
        child->update(elapsedTime);
}

void Widget::draw(Graphics &graphics)
{
    drawChildren(graphics);
}

void Widget::drawChildren(Graphics &graphics)
{
    for (Widget *child : mChildren)
        child->draw(graphics);
}

void Widget::addChild(Widget *child)
{
    if (!IntRect(Vec2i(), mRect.size).contains(child->mRect))
        throw std::runtime_error("Child rect is outside parent rect");

    mChildren.push_back(child);
}

// bool Widget::testShapePoint(const Vec2f &point)
// {
//     return rect.contains(point);
// }