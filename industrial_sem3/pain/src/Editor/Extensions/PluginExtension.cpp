#include "../../Utils/Logger.hpp"
#include "PluginExtension.hpp"

PluginExtension::PluginExtension(const char *name, Plugin *plugin) : Extension(name), mPlugin(plugin)
{
    plugin->ext_enable(name);
}

void *PluginExtension::getFunc(const char *funcName)
{
    void *func = mPlugin->ext_get_func(mName, funcName);
    if (func == nullptr)
    {
        Logger::log(LogLevel::Warning, "Someone tried to access %s from extension %s, but it's not present",
                    funcName, mName);
    }

    return func;
}

void *PluginExtension::getInterface(const char *interfaceName)
{
    void *interface = mPlugin->ext_get_interface(mName, interfaceName);
    if (interface == nullptr)
    {
        Logger::log(LogLevel::Warning, "Someone tried to access %s from extension %s, but it's not present",
                    interfaceName, mName);
    }
    
    return interface;
}