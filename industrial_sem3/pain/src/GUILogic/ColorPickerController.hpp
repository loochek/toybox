#ifndef COLOR_PICKER_CONTROLLER_HPP
#define COLOR_PICKER_CONTROLLER_HPP

#include "../LGL/LGL.hpp"
#include "../GUILogic/KeyColorChangedDelegate.hpp"
#include "../GUIElements/KeyColorPicker.hpp"
#include "../GUIElements/MainColorPicker.hpp"

class ColorPickerController : public KeyColorChangedDelegate
{
public:
    ColorPickerController() = delete;
    ColorPickerController(MainColorPicker *mainColorPicker, KeyColorPicker *keyColorPicker) :
        mMainColorPicker(mainColorPicker), mKeyColorPicker(keyColorPicker) {};

    virtual void onKeyColorChanged(const LGL::Color &color) override
    {
        mMainColorPicker->setKeyColor(color);
    }

private:
    MainColorPicker *mMainColorPicker;
    KeyColorPicker  *mKeyColorPicker;
};

#endif