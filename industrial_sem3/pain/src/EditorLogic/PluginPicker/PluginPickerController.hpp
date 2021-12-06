#ifndef PLUGIN_PICKER_CONTROLLER_HPP
#define PLUGIN_PICKER_CONTROLLER_HPP

#include "../../EditorWidgets/PluginPicker.hpp"
#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "PluginChangedDelegate.hpp"

class PluginPickerController : public ButtonDelegate
{
public:
    PluginPickerController() = delete;
    PluginPickerController(PluginPicker *picker) : mPicker(picker) {};

    // Buttons callback
    virtual void onClick(uint64_t userData) override
    {
        if (mPicker->mDelegate != nullptr)
            mPicker->mDelegate->onPluginChange(mPicker->mPlugins[userData], mPicker->mUserData);
    }

protected:
    PluginPicker *mPicker;
};

#endif