#ifndef WINDOW_HIDE_DELEGATE_HPP
#define WINDOW_HIDE_DELEGATE_HPP

#include "../../BaseGUI/Window.hpp"
#include "../BaseButton/ButtonDelegate.hpp"

class WindowHideDelegate : public ButtonDelegate
{
public:
    WindowHideDelegate() = delete;
    WindowHideDelegate(Window *window) : mWindow(window) {};

    virtual void onClick(int userData) override
    {
        mWindow->scheduleForDisable();
    }

protected:
    Window *mWindow;
};

#endif