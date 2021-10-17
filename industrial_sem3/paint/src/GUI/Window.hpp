#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Widget.hpp"

class WindowBody;
class WindowHeader;

class Window : public Widget
{
public:
    Window() = delete;
    Window(const IntRect &windowRect, Widget *parent = nullptr);

    virtual void redrawThis() override;
};

#endif