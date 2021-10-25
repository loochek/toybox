#ifndef SLIDER_SIDE_BUTTONS_DELEGATE
#define SLIDER_SIDE_BUTTONS_DELEGATE

#include "ButtonDelegate.hpp"
#include "../GUIBase/Slider.hpp"

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
            mSlider->mValue = std::max(mSlider->mValue - 1, 0);
            break;

        case SliderButton::Increase:
            mSlider->mValue = std::min(mSlider->mValue + 1, mSlider->mMaxValue);
            break;
        }
    }

protected:
    Slider *mSlider;
};

#endif