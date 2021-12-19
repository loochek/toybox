#include "../../Utils/Logger.hpp"
#include "ExtensionManager.hpp"
#include "PluginExtension.hpp"

ExtensionManager::~ExtensionManager()
{
    for (Extension *ext : mExtensions)
        delete ext;
}

void ExtensionManager::registerExtension(const char *name, Plugin *plugin)
{
    mExtensions.push_back(new PluginExtension(name, plugin));
    Logger::log(LogLevel::Info, "Registered extension %s", name);
}

bool ExtensionManager::isExtensionPresent(const char *name)
{
    for (Extension *ext : mExtensions)
    {
        if (strcmp(ext->getName(), name) == 0)
            return true;
    }

    return false;
}

void *ExtensionManager::getFunc(const char *extName, const char *funcName)
{
    for (Extension *ext : mExtensions)
    {
        if (strcmp(ext->getName(), extName) == 0)
            return ext->getFunc(funcName);
    }

    Logger::log(LogLevel::Warning, "Someone tried to work with extension %s, but it's not present", extName);
    return nullptr;
}

void *ExtensionManager::getInterface(const char *extName, const char *interfaceName)
{
    for (Extension *ext : mExtensions)
    {
        if (strcmp(ext->getName(), extName) == 0)
            return ext->getInterface(interfaceName);
    }

    Logger::log(LogLevel::Warning, "Someone tried to work with extension %s, but it's not present", extName);
    return nullptr;
}