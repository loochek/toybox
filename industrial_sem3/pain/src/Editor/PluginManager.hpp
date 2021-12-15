#ifndef PLUGIN_MANAGER_HPP
#define PLUGIN_MANAGER_HPP

#include <vector>
#include "../LGL/LGL.hpp"
#include "EditorPluginAPI/plugin_std.hpp"

typedef const PUPPY::PluginInterface Plugin;

class Canvas;
class PaintController;
class AppInterfaceImpl;

/**
 * Singleton which stores plugins and responsible for providing app interface to plugins
 */
class PluginManager
{
public:
    void init(PaintController *controller);
    void loadPlugin(const char *fileName);
    void deinit();

    Plugin *getPlugin(int idx);
    int getPluginsCount() { return mPlugins.size(); };

    static PluginManager *getInstance();

private:
    PluginManager();
    ~PluginManager();

private:
    std::vector<Plugin*> mPlugins;
    std::vector<void*> mLibraryHandles;

    bool mInitedOnce;
    AppInterfaceImpl *mAppInterface;
};

#endif