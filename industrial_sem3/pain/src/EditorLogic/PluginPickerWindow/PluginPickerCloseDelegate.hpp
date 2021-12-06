#ifndef PLUGIN_PICKER_CLOSE_DELEGATE_HPP
#define PLUGIN_PICKER_CLOSE_DELEGATE_HPP

#include "../../EditorWidgets/PluginPickerWindow.hpp"
#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "../PaintController.hpp"

class PluginPickerCloseDelegate : public ButtonDelegate
{
public:
    PluginPickerCloseDelegate() = delete;
    PluginPickerCloseDelegate(PaintController *controller, PluginPickerWindow *window) :
        mWindow(window), mController(controller) {};

    virtual void onClick(uint64_t userData) override
    {
        mWindow->scheduleForDeletion();
        mController->onPluginPickerClose(mWindow);
    }

protected:
    PaintController *mController;
    PluginPickerWindow *mWindow;
};

#endif