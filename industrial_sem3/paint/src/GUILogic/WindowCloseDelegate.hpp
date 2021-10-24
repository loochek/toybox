 
#ifndef WINDOW_CLOSE_DELEGATE_HPP
#define WINDOW_CLOSE_DELEGATE_HPP

#include "ButtonDelegate.hpp"
#include "../GUIBase/Canvas.hpp"
#include "../GUIBase/Window.hpp"

class WindowCloseDelegate : public ButtonDelegate
{
public:
    WindowCloseDelegate() = delete;
    WindowCloseDelegate(Window *window) : mWindow(window) {};

    virtual void onClick(int userData) override
    {
        mWindow->onDestroy();
        
        if (mWindow->getParent() != nullptr)
            mWindow->getParent()->onChildDestroy(mWindow);
    }

protected:
    Window *mWindow;
};

#endif