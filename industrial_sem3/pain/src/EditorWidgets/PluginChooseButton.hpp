#ifndef PLUGIN_CHOOSE_BUTTON_HPP
#define PLUGIN_CHOOSE_BUTTON_HPP

#include "../LGL/LGL.hpp"
#include "../Editor/PluginManager.hpp"
#include "../Editor/AppInterface/RenderTarget.hpp"
#include "../BaseGUI/Button.hpp"

class PluginChooseButton : public Button
{
public:
    PluginChooseButton() = delete;
    PluginChooseButton(const IntRect &widgetRect, Plugin *plugin, Widget *parent = nullptr);

    virtual ~PluginChooseButton() {};

protected:
    virtual void onRedrawThis() override;

protected:
    Plugin *mPlugin;

    const LGL::Texture *mDefaultIconTexture;
    const RenderTargetImpl *mPluginIcon;
    Vec2i mIconPos;
};

#endif