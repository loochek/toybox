#include "CanvasWidget.hpp"

CanvasWidget::CanvasWidget(const IntRect &widgetRect, Widget *parent) : 
    Widget(widgetRect, parent), mCanvas(widgetRect.size),
    mToolActive(false), mDrawToolPreview(false)
{
}

void CanvasWidget::onRedrawThis()
{
    mTexture.drawRenderTexture(*mCanvas.getCurrentState(), Vec2f());
}

void CanvasWidget::onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mDrawToolPreview = true;
}

void CanvasWidget::onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    if (mToolActive)
        mCanvas.onMouseMove(localMousePos);
}

void CanvasWidget::onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mCanvas.onMouseClicked(localMousePos);
    mToolActive = true;
}

void CanvasWidget::onMouseReleased(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    mCanvas.onMouseReleased(localMousePos);
    mToolActive = false;
}

void CanvasWidget::onMouseHoverEnd()
{
    mDrawToolPreview = false;
}