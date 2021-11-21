#ifndef TOOL_CHANGED_DELEGATE_HPP
#define TOOL_CHANGED_DELEGATE_HPP

class Tool;

class ToolChangedDelegate
{
public:
    virtual ~ToolChangedDelegate() {};

    virtual void onToolChange(Tool *newTool, int userData) = 0;
};

#endif