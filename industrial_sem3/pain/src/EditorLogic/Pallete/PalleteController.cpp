#include "PalleteController.hpp"
#include "../../BaseGUI/Label.hpp"
#include "../../EditorWidgets/KeyColorPicker.hpp"
#include "../../EditorWidgets/MainColorPicker.hpp"
#include "../../EditorWidgets/Pallete.hpp"

PalleteController::PalleteController(Pallete *pallete) : mPallete(pallete)
{
}

void PalleteController::onColorChange(const LGL::Color &color, int userData)
{
    switch ((ColorPickerType)userData)
    {
    case ColorPickerType::Key:
        mPallete->mMainColorPicker->setKeyColor(color);
        break;

    case ColorPickerType::Main:
        mCurrColor = color.applyAlpha(mCurrColor.a);
        
        if (mPallete->mDelegate != nullptr)
            mPallete->mDelegate->onColorChange(mCurrColor, mPallete->mUserData);
        
        break;
    }
}

void PalleteController::onValueChange(int newOpacity, int userData)
{
    mCurrColor = mCurrColor.applyAlpha(newOpacity / (float)EXTERNAL_RGB_BASE);
    if (mPallete->mDelegate != nullptr)
        mPallete->mDelegate->onColorChange(mCurrColor, mPallete->mUserData);

    char label[MAX_LABEL_SIZE + 1] = {0};
    snprintf(label, MAX_LABEL_SIZE, "Opacity: %d%%", newOpacity * 100 / EXTERNAL_RGB_BASE);
    mPallete->mOpacityLabel->setText(label);
}