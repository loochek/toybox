#include "../EditorWidgets/SplineWidget.hpp"
#include "SplinePoint.hpp"

const Vec2i SplinePoint::DRAG_AREA_SIZE = Vec2i(30, 30);
const Vec2i SplinePoint::POINT_SIZE     = Vec2i(10, 10);

SplinePoint::SplinePoint(const Vec2i &pointPos, SplineWidget *parent) :
    DragArea(IntRect(pointPos - DRAG_AREA_SIZE / 2, DRAG_AREA_SIZE), this, parent)
{
}

void SplinePoint::onRedrawThis()
{
    mTexture.drawRect(IntRect(DRAG_AREA_SIZE / 2 - POINT_SIZE / 2, POINT_SIZE), LGL::Color::Red);
}