#include "PaintWindow.hpp"
#include "../GUIBase/Canvas.hpp"

PaintWindow::PaintWindow(const IntRect &canvasRect, Widget *parent) : Window(canvasRect, parent)
{
    setTitle("Paint");

    mCanvas = new Canvas(IntRect(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT), canvasRect.size),
                         this);
    addChild(mCanvas);
}