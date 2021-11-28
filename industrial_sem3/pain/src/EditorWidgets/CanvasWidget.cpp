#include "../TextureManager.hpp"
#include "CanvasWidget.hpp"

CanvasWidget::CanvasWidget(const IntRect &widgetRect, Widget *parent) : 
    Widget(widgetRect, parent), mCanvas(widgetRect.size),
    mToolActive(false), mDrawToolPreview(false)
{
    mBackgroundTexture = TextureManager::getInstance()->getTexture("transparency_background");
    if (mBackgroundTexture == nullptr)
        throw std::runtime_error("Canvas widget texture is not loaded");
}

void CanvasWidget::onRedrawThis()
{
    // Background
    mTexture.drawTexture(*mBackgroundTexture, Vec2i(0, 0), IntRect(Vec2i(), mRect.size));

    // Layers
    for (int idx = 0; idx < mCanvas.getLayersCount(); idx++)
    {
        mTexture.drawRenderTexture(*mCanvas.getLayer(idx), Vec2f());

        if (idx == mCanvas.getActiveLayer())
            mTexture.drawRenderTexture(*mCanvas.getPreviewLayer(), Vec2i());
    }
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