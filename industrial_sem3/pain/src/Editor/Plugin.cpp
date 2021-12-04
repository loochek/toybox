#include <dlfcn.h>
#include "../Utils/FormattedError.hpp"
#include "../Utils/Logger.hpp"
#include "Plugin.hpp"

typedef const PPluginInterface* (*PIFunc)();

Plugin *Plugin::sCurrInitPlugin = nullptr;

Plugin::Plugin(const char *fileName, const PAppInterface *appInterface) : mConfigWindow(nullptr)
{    
    mLibraryHandle = dlopen(fileName, RTLD_NOW);
    if (mLibraryHandle == nullptr)
        throw FormattedError("unable to load plugin %s: %s", fileName, dlerror());

    PIFunc getPluginInterface = (PIFunc)dlsym(mLibraryHandle, PGET_INTERFACE_FUNC);
    if (getPluginInterface == nullptr)
    {
        FormattedError error("unable to load %s from %s: %s", PGET_INTERFACE_FUNC, fileName, dlerror());
        dlclose(mLibraryHandle);
        sCurrInitPlugin = nullptr;
        throw error;
    }

    mPluginInterface = getPluginInterface();

    sCurrInitPlugin = this;

    PPluginStatus status = mPluginInterface->general.init(appInterface);
    if (status != PPS_OK)
    {
        FormattedError error("plugin %s initialization failed", fileName);
        dlclose(mLibraryHandle);
        sCurrInitPlugin = nullptr;
        throw error;
    }

    sCurrInitPlugin = nullptr;
}

Plugin::~Plugin()
{
    if (dlclose(mLibraryHandle) != 0)
        Logger::log(LogLevel::Warning, "unable to close plugin: %s", dlerror());
}