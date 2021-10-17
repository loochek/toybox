#ifndef WINDOW_HEADER_HPP
#define WINDOW_HEADER_HPP

#include "Widget.hpp"

class Window;

class WindowHeader : public Widget
{
public:
    WindowHeader() = delete;
    WindowHeader(Window *parent);

    virtual void redrawThis() override;
};

#endif