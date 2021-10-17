#ifndef MAIN_WIDGET_HPP
#define MAIN_WIDGET_HPP

#include "Widget.hpp"

class MainWidget : public Widget
{
public:
    MainWidget(const IntRect &widgetRect, Widget *parent = nullptr);
};

#endif