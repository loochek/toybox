#ifndef COLOR_PICKER_WINDOW_HPP
#define COLOR_PICKER_WINDOW_HPP

#include "../GUIBase/Window.hpp"

class ColorPicker;
class Canvas;
class ColorPickerCloseDelegate;
class PaintController;

/**
 * Canvas color picker window
 */
class ColorPickerWindow : public Window
{
public:
    ColorPickerWindow() = delete;
    ColorPickerWindow(const Vec2f &pickerPos, PaintController *controller, Widget *parent = nullptr);

    virtual ~ColorPickerWindow();

    /**
     * \return Color picker widget
     */
    ColorPicker *getColorPicker() { return mColorPicker; };

private:
    ColorPicker *mColorPicker;
    ColorPickerCloseDelegate *mCloseButtonDelegate;
};

#endif