#ifndef PLUGIN_SETTINGS_NOTIFIER_HPP
#define PLUGIN_SETTINGS_NOTIFIER_HPP

#include "../../EditorWidgets/PluginConfigWindow.hpp"
#include "../../BaseGUILogic/Slider/SliderDelegate.hpp"
#include "../../BaseGUILogic/TextBox/TextBoxDelegate.hpp"
#include "../../EditorLogic/ColorChangeDelegate.hpp"

class PluginSettingsNotifier : public SliderDelegate, public TextBoxDelegate, public ColorChangeDelegate
{
public:
    PluginSettingsNotifier() = delete;
    PluginSettingsNotifier(Plugin *plugin) : mPlugin(plugin) {};

    // Slider callback
    virtual void onValueChange(int newValue, int userData) override
    {
        notify();
    }

    // Text box callback
    virtual void onTextChange(const char *newText, int userData) override
    {
        notify();
    }

    // Pallete callback
    virtual void onColorChange(const LGL::Color &color, int userData) override
    {
        notify();
    }

protected:
    void notify()
    {
        mPlugin->onSettingsUpdate();
    }

protected:
    Plugin *mPlugin;
};

#endif