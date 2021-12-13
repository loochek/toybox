#ifndef PCOLOR_PICKER_DELEGATE
#define PCOLOR_PICKER_DELEGATE

#include "../PluginColorPicker.hpp"
#include "../../../../EditorLogic/ColorChangeDelegate.hpp"

class PColorPickerDelegate : public ColorChangeDelegate
{
public:
    PColorPickerDelegate() = delete;
    PColorPickerDelegate(PluginColorPickerIntl *colorPicker) : mColorPicker(colorPicker) {};

    virtual void onColorChange(const LGL::Color &color, uint64_t userData) override
    {
        PUPPY::ColorPicker::HandlerType &handler = mColorPicker->getImpl()->get_handler();
        if (handler)
            handler(toPluginColor(mColorPicker->getColor()));
    }

protected:
    PluginColorPickerIntl *mColorPicker;
};

#endif