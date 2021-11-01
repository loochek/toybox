#ifndef SIZE_PICKER_WINDOW_HPP
#define SIZE_PICKER_WINDOW_HPP

#include "../GUIBase/Window.hpp"

class SizePicker;
class Canvas;
class SizePickerCloseDelegate;
class PaintController;

class SizePickerWindow : public Window
{
public:
    SizePickerWindow() = delete;
    SizePickerWindow(const Vec2f &pickerPos, PaintController *controller, Widget *parent = nullptr);

    virtual ~SizePickerWindow();

    /**
     * \return Color picker widget
     */
    SizePicker *getSizePicker() { return mSizePicker; };

private:
    SizePicker *mSizePicker;
    SizePickerCloseDelegate *mCloseButtonDelegate;
};

#endif