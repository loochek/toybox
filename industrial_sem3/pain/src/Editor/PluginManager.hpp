#ifndef PLUGIN_MANAGER_HPP
#define PLUGIN_MANAGER_HPP

#include <vector>
#include "Plugin.hpp"

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

    Plugin *getPlugin(int idx);
    int getPluginsCount() { return mPlugins.size(); };

    static PluginManager *getInstance();

private:
    PluginManager();

public:
    // Variables to be used by PAppInterface functions implementations

    Vec2i mCurrCanvasSize;

    LGL::Color mCurrColor;
    float mCurrSize;

    LGL::RenderTexture *mCurrMainLayer;
    LGL::RenderTexture *mCurrPreviewLayer;

private:
    std::vector<Plugin> mPlugins;

    PAppInterface mAppInterface;
};

#endif