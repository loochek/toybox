#ifndef SPLINE_WIDGET_HPP
#define SPLINE_WIDGET_HPP

#include "../../Editor/EditorPluginAPI/plugin_std.hpp"

class SplineWidget : public PUPPY::Widget
{
public:
    virtual float get_value(float x) = 0;
};

#endif