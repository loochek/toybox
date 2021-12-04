#ifndef PLUGIN_CONFIG_WINDOW_HPP
#define PLUGIN_CONFIG_WINDOW_HPP

#include "../BaseGUI/Window.hpp"
#include "../Editor/EditorPluginAPI/plugin_std.hpp"

class PaintController;
class PluginConfigWindowController;

/**
 * Flexible window with an arbitrary set of plugin settings
 */
class PluginConfigWindow : public Window
{
public:
    PluginConfigWindow() = delete;
    PluginConfigWindow(const Vec2i &windowPos, PaintController *controller, Widget *parent = nullptr);

    virtual ~PluginConfigWindow();

    /**
     * Adds a new parameter to the window.
     * The window decides how to arrange parameters by itself.
     * 
     * \param type Parameter type
     * \param name Parameter visible name
     * \return Handle which can be used for calling getParameter function
     */
    void *addParameter(PSettingType type, const char *name);

    /**
     * Returns a requested parameter. Nothing is written if the handle is unknown to the window
     * 
     * \param handle Parameter handle
     * \param structPtr Structure to write into (e.g. PTextFieldSetting, etc). Type is defined by handle. 
     * Caller should know the type of the parameter corresponding to handle
     */
    void getParameter(void *handle, void *structPtr);

private:
    void *addSlider1D(const char *name);
    void *addTextField(const char *name);
    void *addColorPicker(const char *name);
    void addLabel(const char *text);

private:
    PluginConfigWindowController *mController;

    float mNewParamVertOffset;
    float mNewParamHorizOffset;
};

#endif