#ifndef PAINT_MAIN_WINDOW_HPP
#define PAINT_MAIN_WINDOW_HPP

#include "../BaseGUI/WindowManager.hpp"

class PaintController;
class ButtonBar;

class PaintMainWindow : public WindowManager
{
public:
    PaintMainWindow() = delete;
    PaintMainWindow(const IntRect &widgetRect, Widget *parent = nullptr);
    ~PaintMainWindow();

protected:
    virtual void onRedrawThis() override;

protected:
    PaintController *mPaintController;

    ButtonBar *mMenuBar;
    ButtonBar *mTaskBar;

    const LGL::Texture *mWallpaperTexture;

    friend class PaintController;
};

#endif