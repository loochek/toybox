#include <stdexcept>
#include "Widget.hpp"

Widget::Widget(const IntRect &widgetRect, Widget *parent) : mRect(widgetRect), mTexture(widgetRect.size),
                                                            mWidgetUnderMouse(nullptr), mMousePressed(false)
{
};

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

void Widget::addChild(Widget *child)
{
    mChildren.push_back(child);
}

void Widget::onMouseDrag(const Vec2i &mousePosition)
{
    if (mWidgetUnderMouse != nullptr && mWidgetUnderMouse->getRect().contains(mousePosition))
    {
        mWidgetUnderMouse->onMouseDrag(mousePosition - mWidgetUnderMouse->getRect().position);
        return;
    }
    else if (mWidgetUnderMouse != nullptr)
    {
        // mouse is gone outside child 

        if (mMousePressed)
            mWidgetUnderMouse->onMouseReleased();

        mWidgetUnderMouse->onMouseHoverEnd();
    }

    for (Widget *child : mChildren)
    {
        if (child->getRect().contains(mousePosition))
        {
            child->onMouseHoverBegin(mousePosition - child->getRect().position);
            if (mMousePressed)
                child->onMouseClicked();

            mWidgetUnderMouse = child;
            return;
        }
    }

    mWidgetUnderMouse = nullptr;
}

void Widget::onMouseHoverBegin(const Vec2i &mousePosition)
{
    printf("onMouseHoverBegin\n");
    
    for (Widget *child : mChildren)
    {
        if (child->getRect().contains(mousePosition))
        {
            child->onMouseHoverBegin(mousePosition - child->getRect().position);

            mWidgetUnderMouse = child;
            return;
        }
    }

    mWidgetUnderMouse = nullptr;
}

void Widget::onMouseClicked()
{
    if (mWidgetUnderMouse != nullptr)
        mWidgetUnderMouse->onMouseClicked();
    
    mMousePressed = true;

    printf("onMouseClicked\n");
}

void Widget::onMouseReleased()
{
    if (mWidgetUnderMouse != nullptr)
        mWidgetUnderMouse->onMouseReleased();
    
    mMousePressed = false;

    printf("onMouseReleased\n");
}

void Widget::onMouseHoverEnd()
{
    if (mWidgetUnderMouse != nullptr)
    {
        if (mMousePressed == true)
            mWidgetUnderMouse->onMouseReleased();

        mWidgetUnderMouse->onMouseHoverEnd();
    }

    mWidgetUnderMouse = nullptr;
    mMousePressed = false;

    printf("onMouseHoverEnd\n");
}