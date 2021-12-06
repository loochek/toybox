#ifndef PALLETE_CLOSE_DELEGATE_HPP
#define PALLETE_CLOSE_DELEGATE_HPP

#include "../../EditorWidgets/PalleteWindow.hpp"
#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "../PaintController.hpp"

class PalleteCloseDelegate : public ButtonDelegate
{
public:
    PalleteCloseDelegate() = delete;
    PalleteCloseDelegate(PaintController *controller, PalleteWindow *window) :
        mWindow(window), mController(controller) {};

    virtual void onClick(uint64_t userData) override
    {
        mWindow->scheduleForDeletion();
        mController->onPalleteClose();
    }

protected:
    PaintController *mController;
    PalleteWindow   *mWindow;
};

#endif