#include <algorithm>
#include "../TextureManager.hpp"
#include "WindowManager.hpp"

WindowManager::WindowManager(const IntRect &widgetRect, Widget *parent) : Widget(widgetRect, parent)
{
    mWallpaperTexture = TextureManager::getInstance()->getTexture("wallpaper");
    if (mWallpaperTexture == nullptr)
        throw std::runtime_error("Wallpaper texture is not loaded");
}

void WindowManager::onRedrawThis()
{
    mTexture.drawTexture(*mWallpaperTexture, Vec2i());
}

void WindowManager::onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    if (mChildUnderMouse != nullptr)
    {
        auto childIter = std::find(mChildren.begin(), mChildren.end(), mChildUnderMouse);
        popUp(childIter);

        mChildUnderMouse->onMouseClicked(localMousePos - mChildUnderMouse->getRect().position, globalMousePos);
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