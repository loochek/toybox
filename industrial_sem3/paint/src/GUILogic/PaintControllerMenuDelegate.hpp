#ifndef PAINT_CONTROLLER_MENU_DELEGATE_HPP
#define PAINT_CONTROLLER_MENU_DELEGATE_HPP

#include "ButtonDelegate.hpp"
#include "PaintController.hpp"

enum class MenuAction
{
    NewCanvas,
    OpenColorPicker,
    OpenSizePicker
};

class PaintControllerMenuDelegate : public ButtonDelegate
{
public:
    PaintControllerMenuDelegate() = delete;
    PaintControllerMenuDelegate(PaintController *controller) : mController(controller) {};

    virtual void onClick(int userData) override
    {
        switch ((MenuAction)userData)
        {
        case MenuAction::NewCanvas:
            mController->createCanvas();
            break;

        case MenuAction::OpenColorPicker:
            mController->openColorPicker();
            break;

        case MenuAction::OpenSizePicker:
            mController->openSizePicker();
            break;
        }
    }

protected:
    PaintController *mController;
};

#endif