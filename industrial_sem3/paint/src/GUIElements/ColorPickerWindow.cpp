#include "ColorPickerWindow.hpp"
#include "ColorPicker.hpp"

ColorPickerWindow::ColorPickerWindow(const Vec2f &pickerPos, Canvas *canvas, Widget *parent) :
    Window(IntRect(pickerPos, ColorPicker::PICKER_SIZE), parent)
{
    mColorPicker = new ColorPicker(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT), canvas, this);
    addChild(mColorPicker);
}