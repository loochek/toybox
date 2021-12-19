#ifndef PLUGIN_MANAGER_HPP
#define PLUGIN_MANAGER_HPP

#include <vector>
#include <filesystem>
#include "../LGL/LGL.hpp"
#include "EditorPluginAPI/plugin_std.hpp"

typedef PUPPY::PluginInterface Plugin;

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
    void deinit();

    void loadPlugin(const std::filesystem::path &fileName);
    /// Searches for plugins and tries to load them
    void loadPlugins();

    Plugin *getPlugin(int idx);
    int getPluginsCount() { return mPlugins.size(); };

    void pluginsUpdate(float elapsedTime);

    static PluginManager *getInstance();

private:
    PluginManager();
    ~PluginManager();

private:
    PaintController *mController;

    std::vector<AppInterfaceImpl*> mAppInterfaces;
    std::vector<void*> mLibraryHandles;
    std::vector<Plugin*> mPlugins;

    std::filesystem::path mPluginsFolder;

    bool mInitedOnce;
};

#endif