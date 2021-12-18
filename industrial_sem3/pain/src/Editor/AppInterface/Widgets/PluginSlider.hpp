#ifndef PLUGIN_SLIDER_HPP
#define PLUGIN_SLIDER_HPP

#include "../PluginTypesProxy.hpp"
#include "../../../BaseGUI/Slider.hpp"
#include "../../EditorPluginAPI/lib_std/widgets/collection.h"
#include "PluginWidget.hpp"

class PSliderDelegate;
class PluginSliderImpl;

/**
 * Special window which holds wrapper structure
 */
class PluginSliderIntl : public Slider
{
public:
    PluginSliderIntl(const Vec2i &sliderPos, int sliderSize, PluginSliderImpl *impl, Widget *parent = nullptr);
    ~PluginSliderIntl();

    PluginSliderImpl *getImpl() { return mImpl; };

protected:
    EVENTS_FWD_HEADER()

protected:
    PluginSliderImpl *mImpl;
    PSliderDelegate  *mDelegate;
};

class PluginSliderImpl : public PluginWidgetImpl, public PUPPY::Slider
{
public:
    PluginSliderImpl() = delete;
    PluginSliderImpl(const PUPPY::WBody &body, PUPPY::Widget *parent = nullptr);

    virtual PUPPY::Vec2f get_fraction_2d() override;
    virtual void set_fraction_2d(const PUPPY::Vec2f &frac) override;

    virtual float get_fraction() override;
    virtual void set_fraction(float frac) override;

    virtual void set_handler(const HandlerType &handler_) override { mHandler = handler_; };
    virtual HandlerType &get_handler() override { return mHandler; };

protected:
    HandlerType mHandler;
};

#endif