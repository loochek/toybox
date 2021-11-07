#ifndef SPLINE_WIDGET_HPP
#define SPLINE_WIDGET_HPP

#include <vector>
#include "../GUIBase/Widget.hpp"

class SplinePoint;

class SplineWidget : public Widget
{
public:
    SplineWidget(const IntRect &widgetRect, Widget *parent = nullptr);

    void addPoint(const Vec2i &position);

protected:
    virtual void onRedrawThis() override;
};

#endif