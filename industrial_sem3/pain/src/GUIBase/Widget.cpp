#include <stdexcept>
#include <cassert>
#include "Widget.hpp"

Widget::Widget(const IntRect &widgetRect, Widget *parent) : mRect(widgetRect), mParent(parent),
    mUserData(-1), mTexture(widgetRect.size), mScheduledForDeletion(false),
    mChildUnderMouse(nullptr), mChildInFocus(nullptr), mMousePressed(false)
{
};

Widget::~Widget()
{
    for (Widget *child : mChildren)
        delete child;
}

void Widget::onUpdate(float elapsedTime)
{
    onUpdateThis(elapsedTime);
    for (Widget *child : mChildren)
        child->onUpdate(elapsedTime);
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

void Widget::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    for (auto childIter = mChildren.rbegin(); childIter != mChildren.rend(); childIter++)
    {
        Widget *child = *childIter;
        
        if (child->getRect().contains(localMousePos))
        {
            child->onMouseHoverBegin(localMousePos - child->getRect().position, globalMousePos);

            mChildUnderMouse = child;
            return;
        }
    }

    mChildUnderMouse = nullptr;
}

void Widget::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    for (auto childIter = mChildren.rbegin(); childIter != mChildren.rend(); childIter++)
    {
        Widget *child = *childIter;
        
        if (child->getRect().contains(localMousePos))
        {
            if (child == mChildUnderMouse)
            {
                child->onMouseMove(localMousePos - child->getRect().position, globalMousePos);
                return;
            }
                
            if (mChildUnderMouse != nullptr)
            {
                if (mMousePressed)
                    mChildUnderMouse->onMouseReleased();
                mChildUnderMouse->onMouseHoverEnd();
            }

            child->onMouseHoverBegin(localMousePos - child->getRect().position, globalMousePos);
            if (mMousePressed)
            {
                child->onMouseClicked();
                mChildInFocus = child;
            }

            mChildUnderMouse = child;
            return;
        }
    }

    if (mChildUnderMouse != nullptr && !mChildUnderMouse->getRect().contains(localMousePos))
    {
        if (mMousePressed)
            mChildUnderMouse->onMouseReleased();
        mChildUnderMouse->onMouseHoverEnd();
    }
    
    mChildUnderMouse = nullptr;
}

void Widget::onMouseClicked()
{
    if (mChildUnderMouse != nullptr)
    {
        mChildUnderMouse->onMouseClicked();
        mChildInFocus = mChildUnderMouse;
    }
    else
        mChildInFocus = nullptr;
        
    mMousePressed = true;
}

void Widget::onMouseReleased()
{
    if (mChildUnderMouse != nullptr)
        mChildUnderMouse->onMouseReleased();
    
    mMousePressed = false;
}

void Widget::onMouseScroll(int scrollDelta)
{
    if (mChildUnderMouse != nullptr)
        mChildUnderMouse->onMouseScroll(scrollDelta);
}

void Widget::onMouseHoverEnd()
{
    if (mChildUnderMouse != nullptr)
    {
        if (mMousePressed == true)
            mChildUnderMouse->onMouseReleased();

        mChildUnderMouse->onMouseHoverEnd();
    }

    mChildUnderMouse = nullptr;
    mMousePressed = false;
}

EventResult Widget::onKeyPressed(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    if (mChildInFocus != nullptr)
    {
        EventResult result = mChildInFocus->onKeyPressed(key, modifier);
        if (result == EventResult::Handled)
            return result;
    }

    EventResult result = onKeyPressedThis(key, modifier);
    if (result == EventResult::Handled)
        return result;

    return EventResult::Ignored;
}

EventResult Widget::onKeyReleased(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    if (mChildInFocus != nullptr)
    {
        EventResult result = mChildInFocus->onKeyReleased(key, modifier);
        if (result == EventResult::Handled)
            return result;
    }

    EventResult result = onKeyReleasedThis(key, modifier);
    if (result == EventResult::Handled)
        return result;

    return EventResult::Ignored;
}

void Widget::onDestroy()
{
    for (Widget *child : mChildren)
        child->onDestroy();

    onDestroyThis();
}

void Widget::onChildDestroy(Widget *child)
{
    if (child == mChildUnderMouse)
        mChildUnderMouse = nullptr;

    if (child == mChildInFocus)
        mChildInFocus = nullptr;
}

void Widget::addChild(Widget *child)
{
    mChildren.push_back(child);
}