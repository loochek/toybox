#include <stdexcept>
#include <cassert>
#include "Widget.hpp"

Widget::Widget(const IntRect &widgetRect, Widget *parent) : mRect(widgetRect), mParent(parent),
    mTexture(widgetRect.size), mWidgetUnderMouse(nullptr), mMousePressed(false)
{
};

Widget::~Widget()
{
    for (Widget *child : mChildren)
        delete child;
}

void Widget::onUpdate(const Vec2i &parentAbsPos, float elapsedTime)
{
    onUpdateThis(parentAbsPos, elapsedTime);
    Vec2i currAbsPos = parentAbsPos + mRect.position;
    for (Widget *child : mChildren)
        child->onUpdate(currAbsPos, elapsedTime);
}

void Widget::onRedraw()
{
    mTexture.clear();
    onRedrawThis();
    for (Widget *child : mChildren)
    {
        child->onRedraw();
        mTexture.drawRenderTexture(child->mTexture, child->mRect.position);
    }
}

void Widget::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    if (isChild(mWidgetUnderMouse) && mWidgetUnderMouse->getRect().contains(localMousePos))
    {
        mWidgetUnderMouse->onMouseMove(localMousePos - mWidgetUnderMouse->getRect().position, globalMousePos);
        return;
    }
    else if (isChild(mWidgetUnderMouse))
    {
        // mouse is gone outside child

        if (mMousePressed)
            mWidgetUnderMouse->onMouseReleased();

        mWidgetUnderMouse->onMouseHoverEnd();
    }

    for (auto childIter = mChildren.rbegin(); childIter != mChildren.rend(); childIter++)
    {
        Widget *child = *childIter;

        if (child->getRect().contains(localMousePos))
        {
            if (mWidgetUnderMouse == this)
            {
                if (mMousePressed)
                    onMouseReleasedThis();

                onMouseHoverEndThis();
            }

            child->onMouseHoverBegin(localMousePos - child->getRect().position, globalMousePos);
            if (mMousePressed)
                child->onMouseClicked();

            mWidgetUnderMouse = child;
            return;
        }
    }

    if (mWidgetUnderMouse != this)
    {
        onMouseHoverBeginThis(localMousePos, globalMousePos);
        if (mMousePressed)
            onMouseClickedThis();

        mWidgetUnderMouse = this;
    }
    else
        onMouseMoveThis(localMousePos, globalMousePos);
}

void Widget::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    for (auto childIter = mChildren.rbegin(); childIter != mChildren.rend(); childIter++)
    {
        Widget *child = *childIter;
        
        if (child->getRect().contains(localMousePos))
        {
            child->onMouseHoverBegin(localMousePos - child->getRect().position, globalMousePos);

            mWidgetUnderMouse = child;
            return;
        }
    }

    onMouseHoverBeginThis(localMousePos, globalMousePos);
    mWidgetUnderMouse = this;
}

void Widget::onMouseClicked()
{
    if (isChild(mWidgetUnderMouse))
        mWidgetUnderMouse->onMouseClicked();
    else if (mWidgetUnderMouse != nullptr)
        onMouseClickedThis();
    
    mMousePressed = true;
}

void Widget::onMouseReleased()
{
    if (isChild(mWidgetUnderMouse))
        mWidgetUnderMouse->onMouseReleased();
    else if (mWidgetUnderMouse != nullptr)
        onMouseReleasedThis();
    
    mMousePressed = false;
}

void Widget::onMouseHoverEnd()
{
    if (isChild(mWidgetUnderMouse))
    {
        if (mMousePressed == true)
            mWidgetUnderMouse->onMouseReleased();

        mWidgetUnderMouse->onMouseHoverEnd();
    }
    else if (mWidgetUnderMouse != nullptr)
    {
        if (mMousePressed == true)
            onMouseReleasedThis();

        onMouseHoverEndThis();
    }

    mWidgetUnderMouse = nullptr;
    mMousePressed = false;
}

void Widget::onDestroy()
{
    for (Widget *child : mChildren)
        child->onDestroy();

    onDestroyThis();
}

void Widget::onChildDestroy(Widget *child)
{
    mChildren.remove(child);
}

void Widget::addChild(Widget *child)
{
    mChildren.push_back(child);
}