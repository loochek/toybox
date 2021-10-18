#include "Canvas.hpp"

Canvas::Canvas(const IntRect &widgetRect, Widget *parent) : 
    Widget(widgetRect, parent), mCanvas(widgetRect.size),
    mPenDown(false), mPenSize(2.0f), mDrawingColor(LGL::Color::Black)
{
    mCanvas.clear(LGL::Color::White);
}

void Canvas::redrawThis()
{
    mTexture.drawRenderTexture(mCanvas, Vec2f());
}

void Canvas::onMouseDrag(const Vec2i &mousePosition)
{
    if (mPenDown)
        mCanvas.drawCircle(mousePosition, mPenSize, mDrawingColor);
}

void Canvas::onMouseClicked()
{
    mPenDown = true;
}

void Canvas::onMouseReleased()
{
    mPenDown = false;
}