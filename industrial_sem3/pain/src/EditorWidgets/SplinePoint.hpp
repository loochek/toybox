#ifndef SPLINE_POINT_HPP
#define SPLINE_POINT_HPP

#include "../BaseGUI/DragArea.hpp"

class SplineWidget;

/**
 * Draggable point on the spline
 */
class SplinePoint : public DragArea
{
protected:
    SplinePoint(const Vec2i &pointPos, SplineWidget *parent = nullptr);

    virtual void onRedrawThis() override;

protected:
    static const Vec2i DRAG_AREA_SIZE;
    static const Vec2i POINT_SIZE;

    friend class SplineWidget;
};

#endif