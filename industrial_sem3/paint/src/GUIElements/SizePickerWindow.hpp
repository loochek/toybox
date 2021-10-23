#ifndef SIZE_PICKER_WINDOW_HPP
#define SIZE_PICKER_WINDOW_HPP

#include "../GUIBase/Window.hpp"

class SizePicker;
class Canvas;

class SizePickerWindow : public Window
{
public:
    SizePickerWindow() = delete;

    /**
     * \param pickerPos Size picker position
     * \param canvas Canvas to be under control
     * \param parent Parent widget
     */
    SizePickerWindow(const Vec2f &pickerPos, Canvas *canvas, Widget *parent = nullptr);

    /**
     * \return Color picker widget
     */
    SizePicker *getSizePicker() { return mSizePicker; };

private:
    SizePicker *mSizePicker;
};

#endif