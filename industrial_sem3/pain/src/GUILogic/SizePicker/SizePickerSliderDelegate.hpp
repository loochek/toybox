#ifndef SIZE_PICKER_SLIDER_DELEGATE_HPP
#define SIZE_PICKER_SLIDER_DELEGATE_HPP

#include "../../GUIElements/SizePicker.hpp"
#include "../Slider/SliderDelegate.hpp"
#include "../SizePicker/SizeChangedDelegate.hpp"

class SizePickerSliderDelegate : public SliderDelegate
{
public:
    SizePickerSliderDelegate() = delete;
    SizePickerSliderDelegate(SizePicker *picker) : mPicker(picker) {};

    virtual void onValueChange(int newValue, int userData) override
    {
        if (mPicker->mDelegate != nullptr)
            mPicker->mDelegate->onSizeChange(newValue, mPicker->mUserData);
    }

protected:
    SizePicker *mPicker;
};

#endif