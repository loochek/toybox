#ifndef PLUGIN_EXTENSION_HPP
#define PLUGIN_EXTENSION_HPP

#include "Extension.hpp"
#include "../PluginManager.hpp"

class PluginExtension : public Extension
{
public:
    PluginExtension(const char *name, Plugin *plugin);

    virtual void *getFunc(const char *funcName) override;
    virtual void *getInterface(const char *interfaceName) override;

protected:
    Plugin *mPlugin;
};

#endif