#ifndef PBUTTON_DELEGATE
#define PBUTTON_DELEGATE

#include "../PluginSlider.hpp"
#include "../../../../BaseGUILogic/Slider/SliderDelegate.hpp"

const int SLIDER_PREC = 100;

class PSliderDelegate : public SliderDelegate
{
public:
    PSliderDelegate() = delete;
    PSliderDelegate(PluginSliderIntl *slider) : mSlider(slider) {};

    virtual void onValueChange(int newValue, uint64_t userData) override
    {
        PUPPY::Slider::HandlerType &handler = mSlider->getImpl()->get_handler();
        if (handler)
            handler(PUPPY::Vec2f((float)newValue / SLIDER_PREC, 0));
    }

protected:
    PluginSliderIntl *mSlider;
};

#endif