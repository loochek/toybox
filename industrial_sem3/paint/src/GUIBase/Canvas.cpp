#include "Canvas.hpp"

Canvas::Canvas(const IntRect &widgetRect, Widget *parent) : 
    Widget(widgetRect, parent), mCanvas(widgetRect.size),
    mPenDown(false), mDrawPen(false), mPenSize(2.0f), mDrawingColor(LGL::Color::Black)
{
    mCanvas.clear(LGL::Color::White);
}

void Canvas::onRedrawThis()
{
    mTexture.drawRenderTexture(mCanvas, Vec2f());

    if (mDrawPen)
        mTexture.drawCircle(mPenPosition, mPenSize, mDrawingColor);
}

void Canvas::onMouseMoveThis(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    if (mPenDown)
        mCanvas.drawCircle(localMousePos, mPenSize, mDrawingColor);

    mDrawPen = true;
    mPenPosition = localMousePos;
}

void Canvas::onMouseClickedThis()
{
    mPenDown = true;
}

void Canvas::onMouseReleasedThis()
{
    mPenDown = false;
}

void Canvas::onMouseHoverEndThis()
{
    mDrawPen = false;
}