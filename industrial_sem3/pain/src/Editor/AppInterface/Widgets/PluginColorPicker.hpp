#ifndef PLUGIN_COLOR_PICKER_HPP
#define PLUGIN_COLOR_PICKER_HPP

#include "../PluginTypesProxy.hpp"
#include "../../../EditorWidgets/Pallete.hpp"
#include "../../EditorPluginAPI/lib_std/widgets/collection.h"
#include "PluginWidget.hpp"

class PColorPickerDelegate;
class PluginColorPickerImpl;

/**
 * Special window which holds wrapper structure
 */
class PluginColorPickerIntl : public Pallete
{
public:
    PluginColorPickerIntl(const IntRect &widgetRect, PluginColorPickerImpl *impl, Widget *parent = nullptr);
    ~PluginColorPickerIntl();

    PluginColorPickerImpl *getImpl() { return mImpl; };

protected:
    PluginColorPickerImpl *mImpl;
    PColorPickerDelegate  *mDelegate;
};

class PluginColorPickerImpl : public PluginWidgetImpl, public PUPPY::ColorPicker
{
public:
    PluginColorPickerImpl() = delete;
    PluginColorPickerImpl(const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr);

    virtual void set_handler(const HandlerType &handler_) override { mHandler = handler_; };
    virtual HandlerType &get_handler() override { return mHandler; };

    virtual PUPPY::RGBA get_color() override;
    virtual void set_color(PUPPY::RGBA color) override;

protected:
    HandlerType mHandler;
};

#endif