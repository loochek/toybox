#ifndef EXTENSION_MANAGER_HPP
#define EXTENSION_MANAGER_HPP

#include <vector>
#include "Extension.hpp"
#include "../PluginManager.hpp"

class ExtensionManager
{
public:
    ExtensionManager() {}
    ~ExtensionManager();

    /// Registers plugin as extension implementer
    void registerExtension(const char *name, Plugin *plugin);

    bool isExtensionPresent(const char *name);

    void *getFunc     (const char *extName, const char *funcName);
    void *getInterface(const char *extName, const char *interfaceName);

public:
    std::vector<Extension*> mExtensions;
};

#endif