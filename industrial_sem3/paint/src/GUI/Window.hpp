#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "GUIWidget.hpp"

class Window : public GUIWidget
{
public:
    Window() = delete;
    Window(const Rect &windowRect);
};

#endif