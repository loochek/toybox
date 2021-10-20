 
#ifndef WINDOW_CLOSE_DELEGATE_HPP
#define WINDOW_CLOSE_DELEGATE_HPP

#include "../Utils/Delegate.hpp"
#include "../GUIBase/Canvas.hpp"
#include "../GUIBase/Window.hpp"

class WindowCloseDelegate : public Delegate
{
public:
    WindowCloseDelegate() = delete;
    WindowCloseDelegate(Window *window) : mWindow(window) {};

    virtual void operator()() override
    {
        mWindow->onDestroy();
        
        if (mWindow->getParent() != nullptr)
            mWindow->getParent()->onChildDestroy(mWindow);
    }

protected:
    Window *mWindow;
};

#endif