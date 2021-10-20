#include <algorithm>
#include "WindowManager.hpp"

WindowManager::WindowManager(const IntRect &widgetRect, Widget *parent) : Widget(widgetRect, parent)
{
}

void WindowManager::onMouseMove(const Vec2i &mousePosition, const Vec2i &globalMousePos)
{
    if (mWidgetUnderMouse != nullptr && mWidgetUnderMouse->getRect().contains(mousePosition))
    {
        mWidgetUnderMouse->onMouseMove(mousePosition - mWidgetUnderMouse->getRect().position, globalMousePos);
        return;
    }
    else if (mWidgetUnderMouse != nullptr)
    {
        // mouse is gone outside child 

        if (mMousePressed)
            mWidgetUnderMouse->onMouseReleased();

        mWidgetUnderMouse->onMouseHoverEnd();
    }

    for (auto childIter = mChildren.rbegin(); childIter != mChildren.rend(); childIter++)
    {
        Widget *child = *childIter;

        if (child->getRect().contains(mousePosition))
        {
            child->onMouseHoverBegin(mousePosition - child->getRect().position, globalMousePos);
            if (mMousePressed)
            {
                popUp(--childIter.base());
                child->onMouseClicked();
            }

            mWidgetUnderMouse = child;
            return;
        }
    }

    mWidgetUnderMouse = nullptr;
}

void WindowManager::onMouseHoverBegin(const Vec2i &mousePosition, const Vec2i &globalMousePos)
{    
    for (auto childIter = mChildren.rbegin(); childIter != mChildren.rend(); childIter++)
    {
        Widget *child = *childIter;

        if (child->getRect().contains(mousePosition))
        {
            child->onMouseHoverBegin(mousePosition - child->getRect().position, globalMousePos);

            mWidgetUnderMouse = child;
            return;
        }
    }

    mWidgetUnderMouse = nullptr;
}

void WindowManager::onMouseClicked()
{
    if (mWidgetUnderMouse != nullptr)
    {
        auto childIter = std::find(mChildren.begin(), mChildren.end(), mWidgetUnderMouse);
        popUp(childIter);
        mWidgetUnderMouse->onMouseClicked();
    }
    
    mMousePressed = true;
}

void WindowManager::onMouseReleased()
{
    if (mWidgetUnderMouse != nullptr)
        mWidgetUnderMouse->onMouseReleased();
    
    mMousePressed = false;
}

void WindowManager::onMouseHoverEnd()
{
    if (mWidgetUnderMouse != nullptr)
    {
        if (mMousePressed == true)
            mWidgetUnderMouse->onMouseReleased();

        mWidgetUnderMouse->onMouseHoverEnd();
    }

    mWidgetUnderMouse = nullptr;
    mMousePressed = false;
}

void WindowManager::popUp(std::list<Widget*>::iterator widgetIter)
{
    mChildren.splice(mChildren.end(), mChildren, widgetIter);
}