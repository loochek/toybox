#include <algorithm>
#include "WindowManager.hpp"

WindowManager::WindowManager(const IntRect &widgetRect, Widget *parent) : Widget(widgetRect, parent)
{
}

void WindowManager::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
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
                popUp(--childIter.base());
                child->onMouseClicked();
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

void WindowManager::onMouseClicked()
{
    if (mChildUnderMouse != nullptr)
    {
        auto childIter = std::find(mChildren.begin(), mChildren.end(), mChildUnderMouse);
        popUp(childIter);
        mChildUnderMouse->onMouseClicked();
    }
    
    mMousePressed = true;
}

void WindowManager::popUp(std::list<Widget*>::iterator widgetIter)
{
    mChildren.splice(mChildren.end(), mChildren, widgetIter);
}