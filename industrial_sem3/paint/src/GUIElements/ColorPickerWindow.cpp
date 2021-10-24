#include "ColorPickerWindow.hpp"
#include "ColorPicker.hpp"
#include "../GUIBase/BaseButton.hpp"
#include "../GUILogic/ColorPickerCloseDelegate.hpp"

ColorPickerWindow::ColorPickerWindow(const Vec2f &pickerPos, PaintController *controller, Widget *parent) :
    Window(IntRect(pickerPos, ColorPicker::PICKER_SIZE), parent)
{    
    mColorPicker = new ColorPicker(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT), this);
    addChild(mColorPicker);

    mCloseButtonDelegate = new ColorPickerCloseDelegate(controller, this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

ColorPickerWindow::~ColorPickerWindow()
{
    delete mCloseButtonDelegate;
}