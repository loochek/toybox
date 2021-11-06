#include "PalleteController.hpp"
#include "../../GUIElements/KeyColorPicker.hpp"
#include "../../GUIElements/MainColorPicker.hpp"
#include "../../GUIElements/Pallete.hpp"

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

    mPallete->mDelegate->onColorChange(mCurrColor, mPallete->mUserData);
}