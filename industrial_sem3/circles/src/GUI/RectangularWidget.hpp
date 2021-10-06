#ifndef RECTANGULAR_WIDGET_HPP
#define RECTANGULAR_WIDGET_HPP

#include "GUIWidget.hpp"
#include "../Math/Rect.hpp"

class RectangularWidget : public GUIWidget
{
public:
    /**
     * \return Widget rect
     */
    Rect getRect() { return rect; };

protected:
    RectangularWidget() = delete;
    RectangularWidget(const Rect &rect);

    /**
     * Checks point to be inside widget shape
     * 
     * \param point Point
     */
    bool testShape(const Vec2f &point) override;

protected:
    /// Widget rect
    Rect rect;
};

#endif