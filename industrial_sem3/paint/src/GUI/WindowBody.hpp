#ifndef WINDOW_BODY_HPP
#define WINDOW_BODY_HPP

#include "Widget.hpp"

class WindowBody : public Widget
{
public:
    WindowBody() = delete;
    WindowBody(const IntRect &widgetRect, Widget *parent = nullptr);

    virtual void redrawThis() override;
};

#endif