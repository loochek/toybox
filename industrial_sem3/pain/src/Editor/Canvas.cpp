#include <cassert>
#include "../Utils/FormattedError.hpp"
#include "AppInterface/PluginTypesProxy.hpp"
#include "Canvas.hpp"

Canvas::Canvas(const Vec2i &canvasSize) :
    mCurrTool(nullptr), mSize(canvasSize), mPreviewLayer(canvasSize), mCurrLayer(0)
{
    mLayers.push_back(new LGL::RenderTexture(canvasSize));
}

Canvas::~Canvas()
{
    for (LGL::RenderTexture *layer : mLayers)
        delete layer;
}

void Canvas::onMouseClicked(const Vec2i &position)
{
    if (mCurrTool != nullptr)
        mCurrTool->tool_on_press(toPluginVec(Vec2f(position)));
        
    mOldMousePos = position;
}

void Canvas::onMouseMove(const Vec2i &position)
{
    if (mCurrTool != nullptr)
        mCurrTool->tool_on_move(toPluginVec(Vec2f(mOldMousePos)), toPluginVec(Vec2f(position)));

    mOldMousePos = position;
}

void Canvas::onMouseReleased(const Vec2i &position)
{
    if (mCurrTool != nullptr)
    {
        mCurrTool->tool_on_release(toPluginVec(Vec2f(position)));
        flushPreview();
    }
    
    mOldMousePos = position;
}

void Canvas::applyEffect(Plugin *effect)
{
    assert(effect->get_info()->type == PUPPY::PluginType::EFFECT);

    effect->effect_apply();
}

void Canvas::setTool(Plugin *tool)
{
    assert(tool->get_info()->type == PUPPY::PluginType::TOOL);

    mCurrTool = tool;
}

void Canvas::newLayer(int idx)
{
    assert(idx >= 0 && idx <= mLayers.size());
    mLayers.insert(mLayers.begin() + idx, new LGL::RenderTexture(mSize));
}

void Canvas::deleteLayer(int idx)
{
    assert(idx >= 0 && idx < mLayers.size());

    auto iter = mLayers.begin() + idx;
    delete *iter;
    mLayers.erase(iter);

    if (mLayers.size() == 0)
        mLayers.push_back(new LGL::RenderTexture(mSize));

    if (mCurrLayer >= mLayers.size())
        mCurrLayer = mLayers.size() - 1;
}

void Canvas::setActiveLayer(int idx)
{
    assert(idx >= 0 && idx < mLayers.size());
    mCurrLayer = idx;
}

LGL::RenderTexture *Canvas::getLayer(int idx)
{
    assert(idx >= 0 && idx < mLayers.size());
    return mLayers[idx];
}

void Canvas::flushPreview()
{
    mLayers[mCurrLayer]->setBlendMode(true);
    mLayers[mCurrLayer]->drawRenderTexture(mPreviewLayer, Vec2i());
    mPreviewLayer.clear();
}

void Canvas::undo()
{
    // if (mHistory.size() <= 1)
    //     return;
        
    // delete mHistory.back();
    // mHistory.pop_back();
}

bool Canvas::loadFromFile(const char *fileName)
{
    LGL::Texture image;
    if (!image.loadFromFile(fileName))
        return false;

    mSize = image.getSize();

    mPreviewLayer.resize(mSize);
    mPreviewLayer.clear();

    for (LGL::RenderTexture *layer : mLayers)
        delete layer;

    mLayers.clear();

    LGL::RenderTexture *newLayer = new LGL::RenderTexture(mSize);

    newLayer->drawTexture(image, Vec2i());
    mLayers.push_back(newLayer);
    mCurrLayer = 0;
    
    return true;
}

void Canvas::saveToFile(const char *fileName)
{
    LGL::RenderTexture bakedLayers(mSize);
    for (auto iter = mLayers.rbegin(); iter != mLayers.rend(); iter++)
        bakedLayers.drawRenderTexture(**iter, Vec2i());

    LGL::Texture canvasTexture;
    canvasTexture.loadFromRenderTexture(bakedLayers);

    if (!canvasTexture.copyToImage().saveToFile(fileName))
        throw FormattedError("Unable to save canvas %s", fileName);
}

// LGL::RenderTexture *Canvas::getCurrentState()
// {
//     return mHistory.back();
// }

// void Canvas::pushHistoryState()
// {
//     LGL::RenderTexture *newState = new LGL::RenderTexture(mSize);
//     newState->setBlendMode(false);

//     if (mHistory.size() != 0)
//         newState->drawRenderTexture(*mHistory.back(), Vec2f());

//     mHistory.push_back(newState);
// }