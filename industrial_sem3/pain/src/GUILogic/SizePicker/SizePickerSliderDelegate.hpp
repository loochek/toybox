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
        mPicker->mPreviewBrushSize = newValue + 1;

        char label[MAX_LABEL_SIZE + 1] = {0};
        snprintf(label, MAX_LABEL_SIZE, "Brush size: %d px", mPicker->mPreviewBrushSize);
        mPicker->mLabel->setText(label);

        if (mPicker->mDelegate != nullptr)
            mPicker->mDelegate->onSizeChange(mPicker->mPreviewBrushSize, mPicker->mUserData);
    }

protected:
    SizePicker *mPicker;
};

#endif