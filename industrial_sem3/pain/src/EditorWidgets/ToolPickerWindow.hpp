#ifndef TOOL_PICKER_WINDOW_HPP
#define TOOL_PICKER_WINDOW_HPP

#include "../BaseGUI/Window.hpp"

class ToolPicker;
class ToolPickerCloseDelegate;
class PaintController;

class ToolPickerWindow : public Window
{
public:
    ToolPickerWindow() = delete;
    ToolPickerWindow(const Vec2f &pickerPos, PaintController *controller, Widget *parent = nullptr);

    virtual ~ToolPickerWindow();

    /**
     * \return Tool picker widget
     */
    ToolPicker *getToolPicker() { return mToolPicker; };

private:
    ToolPicker *mToolPicker;
    ToolPickerCloseDelegate *mCloseButtonDelegate;
};

#endif