#include "SizePickerWindow.hpp"
#include "SizePicker.hpp"
#include "../GUIBase/WindowHeader.hpp"

SizePickerWindow::SizePickerWindow(const Vec2f &windowPos, Canvas *canvas, Widget *parent) :
    Window(IntRect(windowPos, SizePicker::PICKER_SIZE + Vec2i(0, WindowHeader::HEADER_HEIGHT)), parent)
{
    mHeader = new WindowHeader(this);
    mHeader->setTitle("");
    addChild(mHeader);

    mSizePicker = new SizePicker(Vec2i(0, WindowHeader::HEADER_HEIGHT), canvas, this);
    addChild(mSizePicker);
}