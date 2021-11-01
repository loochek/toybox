#ifndef COLOR_PICKER_CLOSE_DELEGATE_HPP
#define COLOR_PICKER_CLOSE_DELEGATE_HPP

#include "ButtonDelegate.hpp"
#include "PaintController.hpp"
#include "../GUIElements/ColorPickerWindow.hpp"

class ColorPickerCloseDelegate : public ButtonDelegate
{
public:
    ColorPickerCloseDelegate() = delete;
    ColorPickerCloseDelegate(PaintController *controller, ColorPickerWindow *window) :
        mWindow(window), mController(controller) {};

    virtual void onClick(int userData) override
    {
        mWindow->scheduleForDeletion();
        mController->onColorPickerClose();
    }

protected:
    PaintController *mController;
    ColorPickerWindow *mWindow;
};

#endif