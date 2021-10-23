#include "SizePickerWindow.hpp"
#include "SizePicker.hpp"

SizePickerWindow::SizePickerWindow(const Vec2f &pickerPos, Canvas *canvas, Widget *parent) :
    Window(IntRect(pickerPos, SizePicker::PICKER_SIZE), parent)
{
    mSizePicker = new SizePicker(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT), canvas, this);
    addChild(mSizePicker);
}