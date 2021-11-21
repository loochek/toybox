#include "ToolPickerWindow.hpp"
#include "ToolPicker.hpp"
#include "../BaseGUI/BaseButton.hpp"
#include "../EditorLogic/ToolPickerWindow/ToolPickerCloseDelegate.hpp"

ToolPickerWindow::ToolPickerWindow(const Vec2f &pickerPos, PaintController *controller, Widget *parent) :
    Window(IntRect(pickerPos, ToolPicker::PICKER_SIZE), parent)
{
    setTitle("Tools");

    mToolPicker = new ToolPicker(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT), this);
    addChild(mToolPicker);

    for (Tool *tool : controller->getTools())
        mToolPicker->addTool(tool);

    mCloseButtonDelegate = new ToolPickerCloseDelegate(controller, this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

ToolPickerWindow::~ToolPickerWindow()
{
    delete mCloseButtonDelegate;
}