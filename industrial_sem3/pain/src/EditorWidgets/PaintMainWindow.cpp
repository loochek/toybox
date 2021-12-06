#include "../TextureManager.hpp"
#include "../BaseGUI/ButtonBar.hpp"
#include "../EditorLogic/PaintController.hpp"
#include "../Editor/PluginManager.hpp"
#include "PaintMainWindow.hpp"

PaintMainWindow::PaintMainWindow(const IntRect &widgetRect, Widget *parent) :
    WindowManager(widgetRect, parent)
{
    mWallpaperTexture = TextureManager::getInstance()->getTexture("wallpaper");
    if (mWallpaperTexture == nullptr)
        throw std::runtime_error("Wallpaper texture is not loaded");

    mMenuBar = new ButtonBar(ButtonBarLocation::Top, this);    
    addChild(mMenuBar);

    mTaskBar = new ButtonBar(ButtonBarLocation::Bottom, this);
    addChild(mTaskBar);

    mPaintController = new PaintController(this);
}

void PaintMainWindow::onRedrawThis()
{
    mTexture.drawTexture(*mWallpaperTexture, Vec2i());
}