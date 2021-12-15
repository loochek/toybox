#include "../TextureManager.hpp"
#include "../BaseGUI/ButtonBar.hpp"
#include "../EditorLogic/PaintController.hpp"
#include "../Editor/PluginManager.hpp"
#include "PaintMainWindow.hpp"

const LGL::Color TASK_BAR_COLOR = LGL::Color(0.75f, 0.96f, 0.95f, 0.25f);

PaintMainWindow::PaintMainWindow(const IntRect &widgetRect, Widget *parent) :
    WindowManager(widgetRect, parent)
{
    mWallpaperTexture = TextureManager::getInstance()->getTexture("wallpaper");
    if (mWallpaperTexture == nullptr)
        throw std::runtime_error("Wallpaper texture is not loaded");

    mMenuBar = new ButtonBar(ButtonBarLocation::Top, this);    
    addChild(mMenuBar);

    mTaskBar = new ButtonBar(ButtonBarLocation::Bottom, this);
    mTaskBar->setColor(TASK_BAR_COLOR);
    addChild(mTaskBar);

    mPaintController = new PaintController(this);
}

PaintMainWindow::~PaintMainWindow()
{
    delete mPaintController;
}

void PaintMainWindow::onRedrawThis()
{
    mTexture.drawTexture(*mWallpaperTexture, Vec2i());
}