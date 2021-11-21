#ifndef TOOL_PICKER_CONTROLLER_HPP
#define TOOL_PICKER_CONTROLLER_HPP

#include "../../EditorWidgets/ToolPicker.hpp"
#include "../../BaseGUILogic/BaseButton/ButtonDelegate.hpp"
#include "ToolChangedDelegate.hpp"

class ToolPickerController : public ButtonDelegate
{
public:
    ToolPickerController() = delete;
    ToolPickerController(ToolPicker *picker) : mPicker(picker) {};

    // Buttons callback
    virtual void onClick(int userData) override
    {
        if (mPicker->mDelegate != nullptr)
            mPicker->mDelegate->onToolChange(mPicker->mTools[userData], mPicker->mUserData);
    }

protected:
    ToolPicker *mPicker;
};

#endif