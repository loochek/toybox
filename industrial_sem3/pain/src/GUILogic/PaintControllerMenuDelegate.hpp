#ifndef PAINT_CONTROLLER_MENU_DELEGATE_HPP
#define PAINT_CONTROLLER_MENU_DELEGATE_HPP

#include "BaseButton/ButtonDelegate.hpp"
#include "PaintController.hpp"

enum class MenuAction
{
    NewCanvas,
    OpenPallete,
    OpenSizePicker,
    OpenSplineWindow,
    OpenTextBoxDemo
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

        case MenuAction::OpenSplineWindow:
            mController->openSplineWindow();
            break;

        case MenuAction::OpenTextBoxDemo:
            mController->openTextBoxDemo();
            break;
        }
    }

protected:
    PaintController *mController;
};

#endif