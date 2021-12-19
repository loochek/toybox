#include <cstdarg>
#include <cstring>
#include <cassert>
#include <dlfcn.h>
#include "../Utils/FormattedError.hpp"
#include "../Utils/Logger.hpp"
#include "../EditorLogic/PaintController.hpp"
#include "../BaseGUI/Label.hpp"
#include "AppInterface/AppInterface.hpp"
#include "PluginManager.hpp"

const char *SHARED_LIBRARY_EXT = ".so";
const char *PLUGINS_FOLDER     = "./plugins";

PluginManager::PluginManager() : mAppInterface(nullptr), mInitedOnce(false) {}

PluginManager::~PluginManager()
{
    // Late libraries unloading

    for (int i = 0; i < mPlugins.size(); i++)
    {
        const PUPPY::PluginInterface *plugin = mPlugins[i];

        if (dlclose(mLibraryHandles[i]) != 0)
            Logger::log(LogLevel::Warning, "Unable to close plugin %s", dlerror());
    }
}

void PluginManager::init(PaintController *controller)
{
    assert(!mInitedOnce);

    mPluginsFolder = std::filesystem::path(PLUGINS_FOLDER);

    mAppInterface = new AppInterfaceImpl(controller);
    mInitedOnce = true;
}

void PluginManager::deinit()
{
    // Note than deinitialization of plugins is separated from unloading plugins libraries

    for (int i = 0; i < mPlugins.size(); i++)
    {
        Plugin *plugin = mPlugins[i];

        PUPPY::Status status = plugin->deinit();
        if (status != PUPPY::Status::OK)
            Logger::log(LogLevel::Warning, "Unable to deinit plugin %s", plugin->get_info()->name);
    }

    delete mAppInterface;
}

void PluginManager::loadPlugin(const std::filesystem::path &fileName)
{
    assert(mInitedOnce);

    void *libraryHandle = dlopen(fileName.c_str(), RTLD_NOW);
    if (libraryHandle == nullptr)
        throw FormattedError("Unable to load plugin %s", dlerror());

    PUPPY::PluginGetInterfaceType getPluginInterface =
        (PUPPY::PluginGetInterfaceType)dlsym(libraryHandle, PUPPY::GET_INTERFACE_FUNC);

    if (getPluginInterface == nullptr)
    {
        FormattedError error("Unable to load %s from %s", PUPPY::GET_INTERFACE_FUNC, dlerror());
        dlclose(libraryHandle);
        throw error;
    }

    Plugin *pluginInterface = getPluginInterface();

    PUPPY::Status status = pluginInterface->init(mAppInterface, mPluginsFolder);
    if (status != PUPPY::Status::OK)
    {
        FormattedError error("Plugin %s initialization failed", fileName);
        dlclose(libraryHandle);
        throw error;
    }

    mPlugins.push_back(pluginInterface);
    mLibraryHandles.push_back(libraryHandle);
}

void PluginManager::loadPlugins()
{
    for (const auto &entry : std::filesystem::directory_iterator(mPluginsFolder))
    {
        if (entry.path().extension() == SHARED_LIBRARY_EXT)
        {
            try
            {
                loadPlugin(entry.path());
            }
            catch (const std::exception& error)
            {
                Logger::log(LogLevel::Warning, "%s\n", error.what());
            }
        }
    }
}

Plugin *PluginManager::getPlugin(int idx)
{
    assert(idx >= 0 && idx < mPlugins.size());

    return mPlugins[idx];
}

void PluginManager::pluginsUpdate(float elapsedTime)
{
    for (int i = 0; i < mPlugins.size(); i++)
        mPlugins[i]->on_tick(elapsedTime);
}

PluginManager *PluginManager::getInstance()
{
    static PluginManager instance;
    return &instance;
}