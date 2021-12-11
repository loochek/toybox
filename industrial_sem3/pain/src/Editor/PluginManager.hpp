#ifndef PLUGIN_MANAGER_HPP
#define PLUGIN_MANAGER_HPP

#include <vector>

class PaintController;

/**
 * Singleton which stores plugins and implements PAppInterface functions
 */
class PluginManager
{
public:
    void loadPlugin(const char *fileName);

    void setActiveCanvas(Canvas *canvas);
    void onColorChange(const LGL::Color &color) { mCurrColor = color; };
    void onSizeChange(float size) { mCurrSize = size; };

    const P::PluginInterface *getPluginInterface(int idx);
    int getPluginsCount() { return mPlugins.size(); };

    // Assigns app controller to create windows
    void setPaintController(PaintController *controller) { mPaintController = controller; };

    static PluginManager *getInstance();

private:
    PluginManager();
    ~PluginManager();

private:
    std::vector<const P::PluginInterface *> mPlugins;
    std::vector<void*> mLibraryHandles;

    P::AppInterface *mAppInterface;
    PaintController *mPaintController;

    Vec2i mCurrCanvasSize;

    LGL::Color mCurrColor;
    float mCurrSize;

    LGL::RenderTexture *mCurrMainLayer;
    LGL::RenderTexture *mCurrPreviewLayer;
};

#endif