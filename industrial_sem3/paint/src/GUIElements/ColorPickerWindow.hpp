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

    /**
     * \param windowPos Window position
     * \param canvas Canvas to be under control
     * \param parent Parent widget
     */
    ColorPickerWindow(const Vec2f &windowPos, Canvas *canvas, Widget *parent = nullptr);

    /**
     * \return Color picker widget
     */
    ColorPicker *getColorPicker() { return mColorPicker; };

private:
    ColorPicker *mColorPicker;
};

#endif