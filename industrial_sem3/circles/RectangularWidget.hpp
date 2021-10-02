#ifndef RECTANGULAR_WIDGET_HPP
#define RECTANGULAR_WIDGET_HPP

#include "GUIWidget.hpp"
#include "Rect.hpp"

class RectangularWidget : public GUIWidget
{
protected:
    RectangularWidget() = delete;
    RectangularWidget(const Rect &rect);

    /**
     * Checks point to be inside widget shape
     * 
     * \param point Point
     */
    virtual bool testShape(const Vec2f &point);

public:
    /// Widget rect
    Rect rect;
};

#endif