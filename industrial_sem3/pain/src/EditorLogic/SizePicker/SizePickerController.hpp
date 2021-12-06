#ifndef SIZE_PICKER_CONTROLLER_HPP
#define SIZE_PICKER_CONTROLLER_HPP

#include "../../BaseGUI/Slider.hpp"
#include "../../BaseGUI/TextBox.hpp"
#include "../../EditorWidgets/SizePicker.hpp"
#include "../../BaseGUILogic/Slider/SliderDelegate.hpp"
#include "../../BaseGUILogic/TextBox/TextBoxDelegate.hpp"
#include "../SizePicker/SizeChangedDelegate.hpp"

const int INITIAL_SIZE = 2;
const int MAX_SIZE = 20;

class SizePickerController : public SliderDelegate, public TextBoxDelegate
{
public:
    SizePickerController() = delete;
    SizePickerController(SizePicker *picker) : mPicker(picker) {};

    // Slider callback
    virtual void onValueChange(int newValue, uint64_t userData) override
    {
        mPicker->mCurrBrushSize = newValue + 1;

        char newText[MAX_TEXT_BOX_LEN + 1] = {0};
        snprintf(newText, MAX_TEXT_BOX_LEN, "%d", mPicker->mCurrBrushSize);

        mPicker->mTextBox->setText(newText);
        
        if (mPicker->mDelegate != nullptr)
            mPicker->mDelegate->onSizeChange(mPicker->mCurrBrushSize, mPicker->mUserData);
    }

    // Text box callback
    virtual void onTextChange(const char *newText, uint64_t userData) override
    {
        int typedNum = atoi(newText);
        
        if (typedNum < 1)
            return;
        else if (typedNum > MAX_SIZE)
            typedNum = MAX_SIZE;

        mPicker->mCurrBrushSize = typedNum;
        mPicker->mSlider->setValue(typedNum - 1);
        
        if (mPicker->mDelegate != nullptr)
            mPicker->mDelegate->onSizeChange(mPicker->mCurrBrushSize, mPicker->mUserData);
    }

protected:
    SizePicker *mPicker;
};

#endif