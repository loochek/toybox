#ifndef SIZE_PICKER_CLOSE_DELEGATE_HPP
#define SIZE_PICKER_CLOSE_DELEGATE_HPP

#include "../../EditorWidgets/SizePickerWindow.hpp"
#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "../PaintController.hpp"

class SizePickerCloseDelegate : public ButtonDelegate
{
public:
    SizePickerCloseDelegate() = delete;
    SizePickerCloseDelegate(PaintController *controller, SizePickerWindow *window) :
        mWindow(window), mController(controller) {};

    virtual void onClick(uint64_t userData) override
    {
        mWindow->scheduleForDeletion();
        mController->onSizePickerClose();
    }

protected:
    PaintController *mController;
    SizePickerWindow *mWindow;
};

#endif