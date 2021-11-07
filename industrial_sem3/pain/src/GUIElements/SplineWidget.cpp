#include "../Utils/CatmullRomSpline.hpp"
#include "SplinePoint.hpp"
#include "SplineWidget.hpp"

const float SPLINE_THICKNESS = 2.0f;
const float SPLINE_STEP      = 0.5f;

SplineWidget::SplineWidget(const IntRect &widgetRect, Widget *parent) :
    Widget(widgetRect, parent)
{
}

void SplineWidget::addPoint(const Vec2i &position)
{
    addChild(new SplinePoint(position, this));
}

void SplineWidget::onRedrawThis()
{
    mTexture.drawRect(IntRect(Vec2i(), mRect.size));

    CatmullRomSpline spline;

    for (Widget *child : mChildren)
    {
        SplinePoint *point = (SplinePoint*)child;
        spline.addPoint(point->mRect.position + SplinePoint::DRAG_AREA_SIZE / 2);
    }
        
    float maxParam = spline.getMaxParam();

    for (float t = 0; t <= maxParam; t += SPLINE_STEP)
        mTexture.drawCircle(spline[t], SPLINE_THICKNESS, LGL::Color::Black);
}