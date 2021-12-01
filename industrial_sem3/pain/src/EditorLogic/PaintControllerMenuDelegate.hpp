#ifndef PAINT_CONTROLLER_MENU_DELEGATE_HPP
#define PAINT_CONTROLLER_MENU_DELEGATE_HPP

#include "../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "PaintController.hpp"

enum class MenuAction
{
    NewCanvas,
    OpenPallete,
    OpenSizePicker,
    OpenToolPicker,
    OpenEffectPicker,
    OpenSplineWindow,
    OpenImageOpenWindow
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

        case MenuAction::OpenPallete:
            mController->openPallete();
            break;

        case MenuAction::OpenSizePicker:
            mController->openSizePicker();
            break;

        case MenuAction::OpenToolPicker:
            mController->openToolPicker();
            break;

        case MenuAction::OpenEffectPicker:
            mController->openEffectPicker();
            break;

        case MenuAction::OpenSplineWindow:
            mController->openSplineWindow();
            break;

        case MenuAction::OpenImageOpenWindow:
            mController->openImageOpenWindow();
            break;
        }
    }

protected:
    PaintController *mController;
};

#endif