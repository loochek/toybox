#include "Canvas.hpp"
#include "Tool.hpp"
#include "Effect.hpp"

Canvas::Canvas(const Vec2i &canvasSize) : mTool(nullptr), mSize(canvasSize)
{
    mHistory.push_back(new LGL::RenderTexture(canvasSize));
}

void Canvas::onMouseClicked(const Vec2i &position)
{
    if (mTool != nullptr)
    {
        mHistory.push_back(new LGL::RenderTexture(mSize));
        mTool->onMouseClicked(*this, position);
    }
}

void Canvas::onMouseMove(const Vec2i &position)
{
    if (mTool != nullptr)
        mTool->onMouseMove(*this, position);
}

void Canvas::onMouseReleased(const Vec2i &position)
{
    if (mTool != nullptr)
        mTool->onMouseReleased(*this, position);
}

void Canvas::applyEffect(Effect *effect)
{
    effect->apply(*this);
}

void Canvas::undo()
{
    delete mHistory.back();
    mHistory.pop_back();
}

void Canvas::saveToFile(const char *fileName)
{
    LGL::Texture canvasTexture;
    canvasTexture.loadFromRenderTexture(*mHistory.back());

    if (!canvasTexture.copyToImage().saveToFile(fileName))
        throw std::runtime_error("Unable to save canvas");
}