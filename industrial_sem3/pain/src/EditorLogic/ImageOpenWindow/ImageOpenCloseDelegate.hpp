#ifndef IMAGE_OPEN_CLOSE_DELEGATE_HPP
#define IMAGE_OPEN_CLOSE_DELEGATE_HPP

#include "../../EditorWidgets/ImageOpenWindow.hpp"
#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "../PaintController.hpp"

class ImageOpenCloseDelegate : public ButtonDelegate
{
public:
    ImageOpenCloseDelegate() = delete;
    ImageOpenCloseDelegate(PaintController *controller, ImageOpenWindow *window) :
        mWindow(window), mController(controller) {};

    virtual void onClick(uint64_t userData) override
    {
        mWindow->scheduleForDeletion();
        mController->onImageOpenWindowClose();
    }

protected:
    PaintController *mController;
    ImageOpenWindow *mWindow;
};

#endif