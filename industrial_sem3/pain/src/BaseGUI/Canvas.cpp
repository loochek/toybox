#include "Canvas.hpp"

Canvas::Canvas(const IntRect &widgetRect, Widget *parent) : 
    Widget(widgetRect, parent), mCanvas(widgetRect.size),
    mPenDown(false), mDrawPen(false), mPenSize(2.0f), mDrawingColor(LGL::Color::Black)
{
    mCanvas.clear(LGL::Color::White);
}

void Canvas::saveToFile(const char *fileName)
{
    LGL::Texture canvasTexture;
    canvasTexture.loadFromRenderTexture(mCanvas);

    if (!canvasTexture.copyToImage().saveToFile(fileName))
        throw std::runtime_error("Unable to save canvas");
}

void Canvas::onRedrawThis()
{
    mTexture.drawRenderTexture(mCanvas, Vec2f());

    if (mDrawPen)
        mTexture.drawCircle(mPenPosition, mPenSize, mDrawingColor);
}

void Canvas::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mPenPosition = localMousePos;
    mDrawPen = true;
}

void Canvas::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    if (mPenDown)
    {
        mCanvas.drawLine(mPenPosition, localMousePos, mPenSize, mDrawingColor);
        mCanvas.drawCircle(localMousePos, mPenSize, mDrawingColor);
    }

    mPenPosition = localMousePos;
}

void Canvas::onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mPenDown = true;
}

void Canvas::onMouseReleased(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mPenDown = false;
}

void Canvas::onMouseHoverEnd()
{
    mDrawPen = false;
}