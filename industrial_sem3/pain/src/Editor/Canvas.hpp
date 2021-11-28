#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <deque>
#include "../LGL/LGL.hpp"

class Plugin;

class Canvas
{
public:
    Canvas(const Vec2i &canvasSize);
    ~Canvas();

    void onMouseClicked(const Vec2i &position);
    void onMouseMove(const Vec2i &position);
    void onMouseReleased(const Vec2i &position);

    void applyEffect(Plugin *effect);
    void setTool(Plugin *tool);

    /**
     * Inserts a new layer before idx
     * 
     * \param idx Index of the layer to paste before
     */
    void newLayer(int idx);

    /**
     * Deletes a layer
     * 
     * \param idx Index of the layer to delete
     */
    void deleteLayer(int idx);

    void setActiveLayer(int idx);

    LGL::RenderTexture *getLayer(int idx);
    int getLayersCount() { return mLayers.size(); };

    void undo();
    void saveToFile(const char *fileName);

private:
    //void pushHistoryState();

private:
    std::vector<LGL::RenderTexture*> mLayers;
    LGL::RenderTexture mPreviewLayer;

    int mCurrLayer;
    Plugin *mCurrTool;

    Vec2i mSize;
    Vec2i mOldMousePos;

    friend class PluginManager;
};

#endif