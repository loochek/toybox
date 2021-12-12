#ifndef PLUGIN_PICKER_WINDOW_HPP
#define PLUGIN_PICKER_WINDOW_HPP

#include "../BaseGUI/Window.hpp"
#include "../Editor/PluginManager.hpp"

class PluginPicker;
class PluginPickerCloseDelegate;
class PaintController;

class PluginPickerWindow : public Window
{
public:
    PluginPickerWindow() = delete;

    /**
     * \param pickerPos Widget position
     * \param controller App controller
     * \param pickerType Which type of plugin picker should be created
     */
    PluginPickerWindow(const Vec2f &pickerPos, PaintController *controller,
                       P::PluginType pickerType, Widget *parent = nullptr);

    virtual ~PluginPickerWindow();

    /**
     * \return Plugin picker widget
     */
    PluginPicker *getPluginPicker() { return mPluginPicker; };

private:
    PluginPicker *mPluginPicker;
    PluginPickerCloseDelegate *mCloseButtonDelegate;

    P::PluginType mType;
};

#endif