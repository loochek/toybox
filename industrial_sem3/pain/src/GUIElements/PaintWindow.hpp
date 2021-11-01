#ifndef PAINT_WINDOW_HPP
#define PAINT_WINDOW_HPP

#include "../GUIBase/Window.hpp"

class Canvas;
class PaintWindowCloseDelegate;
class PaintController;

class PaintWindow : public Window
{
public:
    PaintWindow() = delete;
    PaintWindow(const IntRect &canvasRect, PaintController *controller, Widget *parent = nullptr);
    
    virtual ~PaintWindow();

    /**
     * \return Canvas widget
     */
    Canvas *getCanvas() { return mCanvas; };

protected:
    Canvas *mCanvas;
    PaintWindowCloseDelegate *mCloseButtonDelegate;
};

#endif