#ifndef PLUGIN_WINDOW_HPP
#define PLUGIN_WINDOW_HPP

#include "../PluginTypesProxy.hpp"
#include "../../../BaseGUI/Window.hpp"
#include "../../EditorPluginAPI/lib_std/widgets/collection.h"
#include "PluginWidget.hpp"

class PluginWindowImpl;
class WindowHideDelegate;
class PaintController;

/**
 * Special window which holds wrapper structure
 */
class PluginWindowIntl : public Window
{
public:
    PluginWindowIntl(const IntRect &contentRect, PluginWindowImpl *impl, PaintController *mController,
                     Widget *parent = nullptr);
    ~PluginWindowIntl();

    PluginWindowImpl *getImpl() { return mImpl; };

protected:
    virtual void onDestroyThis() override;

    EVENTS_FWD_HEADER()

protected:
    PluginWindowImpl   *mImpl;
    WindowHideDelegate *mCloseButtonDelegate;

    PaintController *mController;
};

class PluginWindowImpl : public PluginWidgetImpl, public PUPPY::Window
{
public:
    PluginWindowImpl() = delete;
    PluginWindowImpl(const char *name, const PUPPY::WBody &body, PaintController *controller,
                     PUPPY::Widget *parent = nullptr);

    virtual bool add_child(PUPPY::Widget *child) override;
    virtual PUPPY::WBody get_body() override;
    virtual void set_body(const PUPPY::WBody &body_) override;

    virtual void set_show_handler(HandlerType &handler_) { mShowHandler = handler_; };
    virtual HandlerType &get_show_handler() override { return mShowHandler; };

    virtual void set_hide_handler(HandlerType &handler_) override { mHideHandler = handler_; };
    virtual HandlerType &get_hide_handler() override { return mHideHandler; };

    virtual bool set_name(const char *name) override;
    virtual const char *get_name() override;

protected:
    HandlerType mShowHandler;
    HandlerType mHideHandler;

    PaintController *mController;
};

#endif