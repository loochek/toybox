#ifndef SIZE_PICKER_WINDOW_HPP
#define SIZE_PICKER_WINDOW_HPP

#include "../GUIBase/Window.hpp"

class SizePicker;
class Canvas;

class SizePickerWindow : public Window
{
public:
    SizePickerWindow() = delete;
    SizePickerWindow(const Vec2f &pickerPos, Widget *parent = nullptr);

    /**
     * \return Color picker widget
     */
    SizePicker *getSizePicker() { return mSizePicker; };

private:
    SizePicker *mSizePicker;
};

#endif