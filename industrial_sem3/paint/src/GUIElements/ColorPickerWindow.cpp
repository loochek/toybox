#include "ColorPickerWindow.hpp"
#include "ColorPicker.hpp"
#include "../GUIBase/WindowHeader.hpp"

ColorPickerWindow::ColorPickerWindow(const Vec2f &windowPos, Canvas *canvas, Widget *parent) :
    Window(IntRect(windowPos, ColorPicker::PICKER_SIZE + Vec2i(0, WindowHeader::HEADER_HEIGHT)), parent)
{
    mHeader = new WindowHeader(this);
    mHeader->setTitle("Color");
    addChild(mHeader);

    mColorPicker = new ColorPicker(Vec2i(0, WindowHeader::HEADER_HEIGHT), canvas, this);
    addChild(mColorPicker);
}