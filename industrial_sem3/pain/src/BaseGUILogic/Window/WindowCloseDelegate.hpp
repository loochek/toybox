#ifndef WINDOW_CLOSE_DELEGATE_HPP
#define WINDOW_CLOSE_DELEGATE_HPP

#include "../../BaseGUI/Window.hpp"
#include "../BaseButton/ButtonDelegate.hpp"

class WindowCloseDelegate : public ButtonDelegate
{
public:
    WindowCloseDelegate() = delete;
    WindowCloseDelegate(Window *window) : mWindow(window) {};

    virtual void onClick(int userData) override
    {
        mWindow->scheduleForDeletion();
    }

protected:
    Window *mWindow;
};

#endif