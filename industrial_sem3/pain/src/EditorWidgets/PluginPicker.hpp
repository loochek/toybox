#ifndef PLUGIN_PICKER_HPP
#define PLUGIN_PICKER_HPP

#include "../BaseGUI/Widget.hpp"
#include "../Editor/Plugin.hpp"

class Plugin;
class PluginChangedDelegate;
class PluginPickerController;

class PluginPicker : public Widget
{
public:
    PluginPicker() = delete;
    PluginPicker(const Vec2i &position, PPluginType pickerType, Widget *parent = nullptr);

    virtual ~PluginPicker();

    void addPlugin(Plugin *plugin);

    /**
     * Sets delegate for the plugin picker
     * It is triggered when selected plugin changed
     * 
     * \param delegate Delegate or null pointer
     */
    void setDelegate(PluginChangedDelegate *delegate) { mDelegate = delegate; }

public:
    static const Vec2i PICKER_INIT_SIZE;

protected:
    PluginChangedDelegate  *mDelegate;
    PluginPickerController *mController;

    PPluginType mType;
    int mButtonsCount;

    std::vector<Plugin*> mPlugins;

    friend class PluginPickerController;
};

#endif