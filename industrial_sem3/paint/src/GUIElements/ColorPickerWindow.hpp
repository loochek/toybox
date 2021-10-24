#ifndef COLOR_PICKER_WINDOW
#define COLOR_PICKER_WINDOW

#include "../GUIBase/Window.hpp"

class ColorPicker;
class Canvas;

/**
 * Canvas color picker window
 */
class ColorPickerWindow : public Window
{
public:
    ColorPickerWindow() = delete;

    ColorPickerWindow(const Vec2f &pickerPos, Widget *parent = nullptr);

    /**
     * \return Color picker widget
     */
    ColorPicker *getColorPicker() { return mColorPicker; };

private:
    ColorPicker *mColorPicker;
};

#endif