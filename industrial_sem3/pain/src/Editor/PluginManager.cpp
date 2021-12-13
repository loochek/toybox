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

typedef const PUPPY::PluginInterface* (*PIFunc)();

PluginManager::PluginManager() : mAppInterface(nullptr) {}

PluginManager::~PluginManager()
{
    for (int i = 0; i < mPlugins.size(); i++)
    {
        const PUPPY::PluginInterface *plugin = mPlugins[i];

        PUPPY::Status status = plugin->deinit();
        if (status != PUPPY::Status::OK)
            Logger::log(LogLevel::Warning, "Unable to deinit plugin %s", plugin->get_info()->name);

        if (dlclose(mLibraryHandles[i]) != 0)
            Logger::log(LogLevel::Warning, "Unable to close plugin %s", dlerror());
    }
}

void PluginManager::init(PaintController *controller)
{
    mAppInterface = new AppInterfaceImpl(controller);
    mInited = true;
}

void PluginManager::loadPlugin(const char *fileName)
{
    assert(mInited);

    void *libraryHandle = dlopen(fileName, RTLD_NOW);
    if (libraryHandle == nullptr)
        throw FormattedError("Unable to load plugin %s", dlerror());

    PIFunc getPluginInterface = (PIFunc)dlsym(libraryHandle, PUPPY::GET_INTERFACE_FUNC);
    if (getPluginInterface == nullptr)
    {
        FormattedError error("Unable to load %s from %s", PUPPY::GET_INTERFACE_FUNC, dlerror());
        dlclose(libraryHandle);
        //sCurrInitPlugin = nullptr;
        throw error;
    }

    Plugin *pluginInterface = getPluginInterface();

    //sCurrInitPlugin = this;

    PUPPY::Status status = pluginInterface->init(mAppInterface);
    if (status != PUPPY::Status::OK)
    {
        FormattedError error("Plugin %s initialization failed", fileName);
        dlclose(libraryHandle);
        //sCurrInitPlugin = nullptr;
        throw error;
    }

    //sCurrInitPlugin = nullptr;
    mPlugins.push_back(pluginInterface);
    mLibraryHandles.push_back(libraryHandle);
}

Plugin *PluginManager::getPlugin(int idx)
{
    assert(idx >= 0 && idx < mPlugins.size());

    return mPlugins[idx];
}

PluginManager *PluginManager::getInstance()
{
    static PluginManager instance;
    return &instance;
}