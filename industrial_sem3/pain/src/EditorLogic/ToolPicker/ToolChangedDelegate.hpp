#ifndef TOOL_CHANGED_DELEGATE_HPP
#define TOOL_CHANGED_DELEGATE_HPP

class Plugin;

class ToolChangedDelegate
{
public:
    virtual ~ToolChangedDelegate() {};

    virtual void onToolChange(Plugin *newTool, int userData) = 0;
};

#endif