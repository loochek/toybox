#include <dlfcn.h>
#include "ToolKeeper.hpp"

typedef Tool* (*CreateToolFunc)();

ToolKeeper::~ToolKeeper()
{
    for (void *handle : mLibraryHandles)
        dlclose(handle);
}

void ToolKeeper::loadTool(const char *fileName)
{
    void *toolLib = dlopen(fileName, RTLD_NOW);
    if (toolLib == nullptr)
        throw std::runtime_error(dlerror());

    CreateToolFunc createToolFunc = (CreateToolFunc)dlsym(toolLib, "createTool");
    if (createToolFunc == nullptr)
    {
        std::runtime_error error(dlerror());
        dlclose(toolLib);
        throw error;
    }

    mTools.push_back(createToolFunc());
}