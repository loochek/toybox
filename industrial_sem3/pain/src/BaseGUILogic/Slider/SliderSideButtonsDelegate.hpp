#ifndef SLIDER_SIDE_BUTTONS_DELEGATE
#define SLIDER_SIDE_BUTTONS_DELEGATE

#include "../../BaseGUI/Slider.hpp"
#include "../BaseButton/ButtonDelegate.hpp"

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

        if (mSlider->mDelegate != nullptr)
            mSlider->mDelegate->onValueChange(mSlider->mValue, mSlider->mUserData);
    }

protected:
    Slider *mSlider;
};

#endif