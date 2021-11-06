#ifndef SLIDER_SIDE_BUTTONS_DELEGATE
#define SLIDER_SIDE_BUTTONS_DELEGATE

#include "../BaseButton/ButtonDelegate.hpp"
#include "../../GUIBase/Slider.hpp"

enum class SliderButton
{
    Decrease,
    Increase
};

class SliderSideButtonsDelegate : public ButtonDelegate
{
public:
    SliderSideButtonsDelegate() = delete;
    SliderSideButtonsDelegate(Slider *slider) : mSlider(slider) {};

    virtual void onClick(int userData) override
    {
        switch ((SliderButton)userData)
        {
        case SliderButton::Decrease:
            mSlider->setValue(mSlider->getValue() - 1);
            break;

        case SliderButton::Increase:
            mSlider->setValue(mSlider->getValue() + 1);
            break;
        }
    }

protected:
    Slider *mSlider;
};

#endif