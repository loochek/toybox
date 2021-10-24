#ifndef SIZE_PICKER_CLOSE_DELEGATE_HPP
#define SIZE_PICKER_CLOSE_DELEGATE_HPP

#include "ButtonDelegate.hpp"
#include "PaintController.hpp"
#include "../GUIElements/SizePickerWindow.hpp"

class SizePickerCloseDelegate : public ButtonDelegate
{
public:
    SizePickerCloseDelegate() = delete;
    SizePickerCloseDelegate(PaintController *controller, SizePickerWindow *window) :
        mWindow(window), mController(controller) {};

    virtual void onClick(int userData) override
    {
        mWindow->scheduleForDeletion();
        mController->onSizePickerClose();
    }

protected:
    PaintController *mController;
    SizePickerWindow *mWindow;
};

#endif