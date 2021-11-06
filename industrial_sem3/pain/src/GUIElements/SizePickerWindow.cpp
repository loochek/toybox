#include "SizePickerWindow.hpp"
#include "SizePicker.hpp"
#include "../GUIBase/BaseButton.hpp"
#include "../GUILogic/SizePickerWindow/SizePickerCloseDelegate.hpp"

SizePickerWindow::SizePickerWindow(const Vec2f &pickerPos, PaintController *controller, Widget *parent) :
    Window(IntRect(pickerPos, SizePicker::PICKER_SIZE), parent)
{
    setTitle("Size");

    mSizePicker = new SizePicker(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT), this);
    addChild(mSizePicker);

    mCloseButtonDelegate = new SizePickerCloseDelegate(controller, this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

SizePickerWindow::~SizePickerWindow()
{
    delete mCloseButtonDelegate;
}