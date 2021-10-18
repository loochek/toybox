#include "PaintWindow.hpp"
#include "../GUIBase/WindowHeader.hpp"
#include "../GUIBase/Canvas.hpp"

PaintWindow::PaintWindow(const IntRect &windowRect, Widget *parent) : Window(windowRect, parent)
{
    mHeader = new WindowHeader(this);
    mHeader->setTitle("Paint");
    addChild(mHeader);

    int headerHeight = mHeader->getRect().size.y;

    mCanvas = new Canvas(IntRect(Vec2i(0, headerHeight), windowRect.size - Vec2i(0, headerHeight)),
                         this);
    addChild(mCanvas);
}