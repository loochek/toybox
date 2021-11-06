#ifndef PAINT_WINDOW_CLOSE_DELEGATE_HPP
#define PAINT_WINDOW_CLOSE_DELEGATE_HPP

#include "../BaseButton/ButtonDelegate.hpp"
#include "../PaintController.hpp"
#include "../../GUIElements/PaintWindow.hpp"

class PaintWindowCloseDelegate : public ButtonDelegate
{
public:
    PaintWindowCloseDelegate() = delete;
    PaintWindowCloseDelegate(PaintController *controller, PaintWindow *window) :
        mWindow(window), mController(controller) {};

    virtual void onClick(int userData) override
    {
        mWindow->scheduleForDeletion();
        mController->onCanvasClose(mWindow);
    }

protected:
    PaintController *mController;
    PaintWindow *mWindow;
};

#endif