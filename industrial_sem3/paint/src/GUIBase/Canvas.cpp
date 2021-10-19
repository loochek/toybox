#include "Canvas.hpp"

Canvas::Canvas(const IntRect &widgetRect, Widget *parent) : 
    Widget(widgetRect, parent), mCanvas(widgetRect.size),
    mPenDown(false), mPenSize(2.0f), mDrawingColor(LGL::Color::Black)
{
    mCanvas.clear(LGL::Color::White);
}

void Canvas::onRedrawThis()
{
    mTexture.drawRenderTexture(mCanvas, Vec2f());
}

void Canvas::onMouseMoveThis(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    if (mPenDown)
        mCanvas.drawCircle(localMousePos, mPenSize, mDrawingColor);
}

void Canvas::onMouseClickedThis()
{
    mPenDown = true;
}

void Canvas::onMouseReleasedThis()
{
    mPenDown = false;
}