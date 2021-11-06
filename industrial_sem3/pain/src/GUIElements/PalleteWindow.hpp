#ifndef PALLETE_WINDOW_HPP
#define PALLETE_WINDOW_HPP

#include "../GUIBase/Window.hpp"

class Pallete;
class PalleteCloseDelegate;
class PaintController;

class PalleteWindow : public Window
{
public:
    PalleteWindow() = delete;
    PalleteWindow(const Vec2f &pickerPos, PaintController *controller, Widget *parent = nullptr);

    virtual ~PalleteWindow();

    /**
     * \return Color picker widget
     */
    Pallete *getPallete() { return mPallete; };

private:
    Pallete *mPallete;
    PalleteCloseDelegate *mCloseButtonDelegate;
};

#endif