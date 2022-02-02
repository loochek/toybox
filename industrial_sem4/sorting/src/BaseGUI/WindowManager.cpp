#include <algorithm>
#include "WindowManager.hpp"

WindowManager::WindowManager(const IntRect &widgetRect, Widget *parent) : Widget(widgetRect, parent)
{
}

void WindowManager::popUp(Widget* widget)
{
    auto iter = std::find(mChildren.begin(), mChildren.end(), widget);
    if (iter == mChildren.end())
        return;

    popUp(iter);
}

void WindowManager::onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    if (mChildInFocus != nullptr && mChildInFocus != mChildUnderMouse)
        mChildInFocus->onKeyboardFocusLost();

    if (mChildUnderMouse != nullptr)
    {
        auto childIter = std::find(mChildren.begin(), mChildren.end(), mChildUnderMouse);
        popUp(childIter);

        mChildUnderMouse->onMouseClicked(localMousePos - mChildUnderMouse->getRect().position, globalMousePos);

        mChildUnderMouse->onKeyboardFocusReceived();
        mChildInFocus = mChildUnderMouse;
    }
    else
        mChildInFocus = nullptr;
        
    mMousePressed = true;
}

void WindowManager::popUp(std::list<Widget*>::iterator widgetIter)
{
    mChildren.splice(mChildren.end(), mChildren, widgetIter);
}