#include "Canvas.hpp"
#include "Tool.hpp"
#include "Effect.hpp"

Canvas::Canvas(const Vec2i &canvasSize) : mTool(nullptr), mSize(canvasSize)
{
    pushHistoryState();
}

Canvas::~Canvas()
{
    for (LGL::RenderTexture *state : mHistory)
        delete state;
}

void Canvas::onMouseClicked(const Vec2i &position)
{
    if (mTool != nullptr)
    {
        pushHistoryState();
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
    if (mHistory.size() <= 1)
        return;
        
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

void Canvas::pushHistoryState()
{
    LGL::RenderTexture *newState = new LGL::RenderTexture(mSize);
    newState->setBlendMode(false);

    if (mHistory.size() != 0)
        newState->drawRenderTexture(*mHistory.back(), Vec2f());

    mHistory.push_back(newState);
}