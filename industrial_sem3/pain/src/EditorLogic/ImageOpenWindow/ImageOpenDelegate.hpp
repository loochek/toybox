#ifndef IMAGE_OPEN_DELEGATE_HPP
#define IMAGE_OPEN_DELEGATE_HPP

#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "../../EditorWidgets/ImageOpenWindow.hpp"
#include "../../BaseGUI/TextBox.hpp"
#include "../PaintController.hpp"

class ImageOpenDelegate : public ButtonDelegate
{
public:
    ImageOpenDelegate(ImageOpenWindow *window, PaintController *controller) :
        mWindow(window), mController(controller) {};

    virtual void onClick(uint64_t userData) override
    {
        mController->openFile(mWindow->mFileNameBox->getText());
    }

public:
    ImageOpenWindow *mWindow;
    PaintController *mController;
};

#endif