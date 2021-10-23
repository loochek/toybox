#ifndef PAINT_WINDOW_HPP
#define PAINT_WINDOW_HPP

#include "../GUIBase/Window.hpp"

class Canvas;

class PaintWindow : public Window
{
public:
    PaintWindow() = delete;
    PaintWindow(const IntRect &canvasRect, Widget *parent = nullptr);

    /**
     * \return Canvas widget
     */
    Canvas *getCanvas() { return mCanvas; };

protected:
    Canvas *mCanvas;
};

#endif