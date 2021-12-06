#ifndef PLUGIN_CHANGED_DELEGATE_HPP
#define PLUGIN_CHANGED_DELEGATE_HPP

class Plugin;

class PluginChangedDelegate
{
public:
    virtual ~PluginChangedDelegate() {};

    virtual void onPluginChange(Plugin *newPlugin, uint64_t userData) = 0;
};

#endif