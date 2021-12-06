#ifndef PAINT_WINDOW_CLOSE_DELEGATE_HPP
#define PAINT_WINDOW_CLOSE_DELEGATE_HPP

#include "../../EditorWidgets/PaintWindow.hpp"
#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "../PaintController.hpp"

class PaintWindowCloseDelegate : public ButtonDelegate
{
public:
    PaintWindowCloseDelegate() = delete;
    PaintWindowCloseDelegate(PaintController *controller, PaintWindow *window) :
        mWindow(window), mController(controller) {};

    virtual void onClick(uint64_t userData) override
    {
        mWindow->scheduleForDeletion();
        mController->onCanvasClose(mWindow);
    }

protected:
    PaintController *mController;
    PaintWindow *mWindow;
};

#endif