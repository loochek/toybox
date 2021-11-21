#ifndef TOOL_PICKER_CLOSE_DELEGATE_HPP
#define TOOL_PICKER_CLOSE_DELEGATE_HPP

#include "../../EditorWidgets/ToolPickerWindow.hpp"
#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "../PaintController.hpp"

class ToolPickerCloseDelegate : public ButtonDelegate
{
public:
    ToolPickerCloseDelegate() = delete;
    ToolPickerCloseDelegate(PaintController *controller, ToolPickerWindow *window) :
        mWindow(window), mController(controller) {};

    virtual void onClick(int userData) override
    {
        mWindow->scheduleForDeletion();
        mController->onToolPickerClose();
    }

protected:
    PaintController *mController;
    ToolPickerWindow *mWindow;
};

#endif