#ifndef PLUGIN_WINDOW_HPP
#define PLUGIN_WINDOW_HPP

#include "../PluginTypesProxy.hpp"
#include "../../../BaseGUI/Window.hpp"
#include "../../EditorPluginAPI/lib_std/widgets/collection.h"
#include "PluginWidget.hpp"

class PluginWindowImpl;

/**
 * Special window which holds wrapper structure
 */
class PluginWindowIntl : public virtual PluginWidgetIntl, public Window
{
public:
    PluginWindowIntl(const IntRect &contentRect, PluginWidgetIntl *parent = nullptr);
};

class PluginWindowImpl : public PluginWidgetImpl, public P::Window
{
public:
    PluginWindowImpl() = delete;
    PluginWindowImpl(const P::WBody &body, PluginWindowIntl *widget, P::Widget *parent = nullptr);

    virtual void set_show_handler(HandlerType &handler_) override;
    virtual HandlerType &get_show_handler() override;

    virtual void set_hide_handler(HandlerType &handler_) override;
    virtual HandlerType &get_hide_handler() override;

    virtual bool set_name(const char *name) override;
    virtual const char *get_name() override;
};

#endif