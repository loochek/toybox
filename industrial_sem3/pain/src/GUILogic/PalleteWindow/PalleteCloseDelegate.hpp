#ifndef PALLETE_CLOSE_DELEGATE_HPP
#define PALLETE_CLOSE_DELEGATE_HPP

#include "../BaseButton/ButtonDelegate.hpp"
#include "../PaintController.hpp"
#include "../../GUIElements/PalleteWindow.hpp"

class PalleteCloseDelegate : public ButtonDelegate
{
public:
    PalleteCloseDelegate() = delete;
    PalleteCloseDelegate(PaintController *controller, PalleteWindow *window) :
        mWindow(window), mController(controller) {};

    virtual void onClick(int userData) override
    {
        mWindow->scheduleForDeletion();
        mController->onPalleteClose();
    }

protected:
    PaintController *mController;
    PalleteWindow   *mWindow;
};

#endif