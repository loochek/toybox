#include "RectangularWidget.hpp"

RectangularWidget::RectangularWidget(const Rect &rect) : rect(rect)
{
}

bool RectangularWidget::testShape(const Vec2f &point)
{
    return rect.contains(point);
}