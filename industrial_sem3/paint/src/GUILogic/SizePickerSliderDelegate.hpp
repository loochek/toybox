#ifndef SIZE_PICKER_SLIDER_DELEGATE_HPP
#define SIZE_PICKER_SLIDER_DELEGATE_HPP

#include "../GUIElements/SizePicker.hpp"
#include "SliderDelegate.hpp"

class SizePickerSliderDelegate : public SliderDelegate
{
public:
    SizePickerSliderDelegate() = delete;
    SizePickerSliderDelegate(SizePicker *picker) : mPicker(picker) {};

    virtual void onValueChanged(int newValue) override
    {
        mPicker->onSizeChanged(newValue);
    }

protected:
    SizePicker *mPicker;
};

#endif