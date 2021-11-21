#ifndef TOOL_PICKER_HPP
#define TOOL_PICKER_HPP

#include "../BaseGUI/Widget.hpp"

class Tool;
class ToolChangedDelegate;
class ToolPickerController;

class ToolPicker : public Widget
{
public:
    ToolPicker() = delete;
    ToolPicker(const Vec2i &position, Widget *parent = nullptr);

    virtual ~ToolPicker();

    void addTool(Tool *tool);

    /**
     * Sets delegate for the tool picker
     * It is triggered when selected tool changed
     * 
     * \param delegate Delegate or null pointer
     */
    void setDelegate(ToolChangedDelegate *delegate) { mDelegate = delegate; }

public:
    static const Vec2i PICKER_SIZE;

protected:
    ToolChangedDelegate  *mDelegate;
    ToolPickerController *mController;

    std::vector<Tool*> mTools;

    friend class ToolPickerController;
};

#endif