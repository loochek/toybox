#ifndef TEXT_BOX_DEMO_WINDOW_HPP
#define TEXT_BOX_DEMO_WINDOW_HPP

#include "../GUIBase/Window.hpp"

class WindowCloseDelegate;

class TextBoxDemoWindow : public Window
{
public:
    TextBoxDemoWindow() = delete;
    TextBoxDemoWindow(const IntRect &windowRect, Widget *parent = nullptr);

    virtual ~TextBoxDemoWindow();
    
protected:
    WindowCloseDelegate *mCloseButtonDelegate;
};

#endif