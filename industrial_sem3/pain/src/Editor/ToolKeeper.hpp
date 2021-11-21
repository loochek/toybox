#ifndef TOOL_KEEPER_HPP
#define TOOL_KEEPER_HPP

#include <vector>
#include "Tool.hpp"

class ToolKeeper
{
public:
    ~ToolKeeper();

    void loadTool(const char *fileName);

    std::vector<Tool*> &getTools() { return mTools; };

private:
    std::vector<Tool*> mTools;
    std::vector<void*> mLibraryHandles;
};

#endif