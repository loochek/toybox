#ifndef PLUGIN_BUTTON_HPP
#define PLUGIN_BUTTON_HPP

#include "../PluginTypesProxy.hpp"
#include "../../../BaseGUI/Button.hpp"
#include "../../EditorPluginAPI/lib_std/widgets/collection.h"
#include "PluginWidget.hpp"

class PButtonDelegate;
class PluginButtonImpl;

/**
 * Special window which holds wrapper structure
 */
class PluginButtonIntl : public Button
{
public:
    PluginButtonIntl(const IntRect &widgetRect, PUPPY::Button *impl, Widget *parent = nullptr);
    ~PluginButtonIntl();

    PUPPY::Button *getImpl() { return mImpl; };

protected:
    EVENTS_FWD_HEADER()

protected:
    PUPPY::Button *mImpl;
    PButtonDelegate  *mDelegate;
};

class PluginButtonImpl : public PluginWidgetImpl, public PUPPY::Button
{
public:
    PluginButtonImpl() = delete;
    PluginButtonImpl(const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr);
    PluginButtonImpl(const PUPPY::WBody &body, const char *label, PUPPY::Widget *parent = nullptr);

    virtual void set_handler(const HandlerType &handler_) override { mHandler = handler_; };
    virtual HandlerType &get_handler() override { return mHandler; };

protected:
    HandlerType mHandler;
};

#endif