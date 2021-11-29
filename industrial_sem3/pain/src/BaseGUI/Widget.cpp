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

void Widget::addChild(Widget *child)
{
    mChildren.push_back(child);
}

void Widget::resize(const Vec2i &newSize)
{
    mRect.size = newSize;
    //mTexture.resize(newSize);
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

void Widget::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    onMouseHoverBeginThis(localMousePos, globalMousePos);

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
}

void Widget::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    onMouseMoveThis(localMousePos, globalMousePos);

    if (mMousePressed)
    {
        if (mChildUnderMouse != nullptr)
            mChildUnderMouse->onMouseMove(localMousePos - mChildUnderMouse->getRect().position, globalMousePos);

        return;
    }

    if (mChildUnderMouse != nullptr && !mChildUnderMouse->getRect().contains(localMousePos))
    {
        mChildUnderMouse->onMouseHoverEnd();
        mChildUnderMouse = nullptr;
    }

    for (auto childIter = mChildren.rbegin(); childIter != mChildren.rend(); childIter++)
    {
        Widget *child = *childIter;
        
        if (child->getRect().contains(localMousePos))
        {
            if (child == mChildUnderMouse)
            {
                mChildUnderMouse->onMouseMove(localMousePos - mChildUnderMouse->getRect().position,
                                              globalMousePos);
                return;
            }
                
            if (mChildUnderMouse != nullptr)
            {
                if (mMousePressed)
                {
                    mChildUnderMouse->onMouseReleased(localMousePos - mChildUnderMouse->getRect().position,
                                                      globalMousePos);
                }
                
                mChildUnderMouse->onMouseHoverEnd();
            }

            child->onMouseHoverBegin(localMousePos - child->getRect().position, globalMousePos);
            mChildUnderMouse = child;
            return;
        }
    }
}

void Widget::onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    onMouseClickedThis(localMousePos, globalMousePos);

    if (mChildInFocus != nullptr && mChildInFocus != mChildUnderMouse)
        mChildInFocus->onKeyboardFocusLost();

    if (mChildUnderMouse != nullptr)
    {
        mChildUnderMouse->onMouseClicked(localMousePos - mChildUnderMouse->getRect().position, globalMousePos);

        mChildUnderMouse->onKeyboardFocusReceived();
        mChildInFocus = mChildUnderMouse;
    }
    else
        mChildInFocus = nullptr;
        
    mMousePressed = true;
}

void Widget::onMouseReleased(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    onMouseReleasedThis(localMousePos, globalMousePos);

    mMousePressed = false;

    if (mChildUnderMouse != nullptr)
    {
        mChildUnderMouse->onMouseReleased(localMousePos - mChildUnderMouse->getRect().position, globalMousePos);

        if (!mChildUnderMouse->getRect().contains(localMousePos))
        {
            mChildUnderMouse->onMouseHoverEnd();
            mChildUnderMouse = nullptr;
        }
        else
            return;
    }

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
}

void Widget::onMouseScroll(int scrollDelta)
{
    if (mChildUnderMouse != nullptr)
        mChildUnderMouse->onMouseScroll(scrollDelta);
}

void Widget::onMouseHoverEnd()
{
    onMouseHoverEndThis();

    if (mChildUnderMouse != nullptr)
        mChildUnderMouse->onMouseHoverEnd();

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

void Widget::onKeyboardFocusReceived()
{
    onKeyboardFocusReceivedThis();
    if (mChildInFocus != nullptr)
        mChildInFocus->onKeyboardFocusReceived();
}

void Widget::onKeyboardFocusLost()
{
    onKeyboardFocusLostThis();
    if (mChildInFocus != nullptr)
        mChildInFocus->onKeyboardFocusLost();
}