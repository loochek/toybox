#include "../Utils/CatmullRomSpline.hpp"
#include "SplinePoint.hpp"
#include "SplineWidget.hpp"

const float SPLINE_THICKNESS = 2.0f;
const float SPLINE_STEP      = 10.0f;
const float POINT_SPAWN_DIST = 8.0f;

SplineWidget::SplineWidget(const IntRect &widgetRect, Widget *parent) :
    Widget(widgetRect, parent)
{
    recalcSpline();
}

void SplineWidget::onRedrawThis()
{
    mTexture.drawRect(IntRect(Vec2i(), mRect.size));
        
    float maxParam = mSpline.getMaxParam();
    Vec2f prevPoint = mSpline[0];
    for (float t = 0; t <= maxParam; t += SPLINE_STEP)
    {
        Vec2f currPoint = mSpline[t];
        mTexture.drawCircle(currPoint, SPLINE_THICKNESS, LGL::Color::Black);
        mTexture.drawLine(prevPoint, currPoint, SPLINE_THICKNESS, LGL::Color::Black);
        prevPoint = currPoint;
    }

    // to avoid flickering with big step

    Vec2f currPoint = mSpline[maxParam];

    mTexture.drawCircle(currPoint, SPLINE_THICKNESS, LGL::Color::Black);
    mTexture.drawLine(prevPoint, currPoint, SPLINE_THICKNESS, LGL::Color::Black);
}

void SplineWidget::onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    if (mChildUnderMouse == nullptr)
    {
        // Go through the spline and add a point
        // if it's allowed by the distance between spline and mouse pointer 
        float maxParam = mSpline.getMaxParam();
        for (float t = 0; t <= maxParam; t += SPLINE_STEP)
        {
            Vec2f currPoint = mSpline[t];

            if ((currPoint - localMousePos).length() < POINT_SPAWN_DIST)
            {
                SplinePoint *point = new SplinePoint(currPoint, this);
                addChild(point);
                
                mChildUnderMouse = point;
                mChildUnderMouse->onMouseHoverBegin(localMousePos - mChildUnderMouse->getRect().position,
                                                    globalMousePos);

                recalcSpline();
                break;
            }
        }
    }

    if (mChildUnderMouse != nullptr)
        mChildUnderMouse->onMouseClicked(localMousePos - mChildUnderMouse->getRect().position, globalMousePos);

    mMousePressed = true;
}

void SplineWidget::recalcSpline()
{
    mChildren.sort([](const Widget *first, const Widget *second) -> bool
    {
        Vec2i thisPos  = first->getRect().position;
        Vec2i otherPos = second->getRect().position;

        if (thisPos.x == otherPos.x)
            return thisPos.y < otherPos.y;

        return thisPos.x < otherPos.x;
    });

    mSpline.clear();

    Vec2f diagVector = Vec2f(mRect.size).normalized();
    diagVector.y *= -1.0f;

    // First base point beyond bottom left corner of the widget
    mSpline.addPoint(Vec2f(0.0f, mRect.size.y) - diagVector);

    // First visible point at bottom left corner of the widget
    mSpline.addPoint(Vec2f(0.0f, mRect.size.y));

    // User-defined points
    for (Widget *child : mChildren)
    {
        SplinePoint *point = (SplinePoint*)child;

        if (!IntRect(Vec2i(), mRect.size).contains(point->mRect.position))
        {
            // Remove point if it goes outside
            point->scheduleForDeletion();
            continue;
        }

        mSpline.addPoint(point->mRect.position + SplinePoint::DRAG_AREA_SIZE / 2);
    }

    // Second visible point at top right corner of the widget
    mSpline.addPoint(Vec2f(mRect.size.x, 0.0f));

    // Second base point beyond bottom left corner of the widget
    mSpline.addPoint(Vec2f(mRect.size.x, 0.0f) + diagVector);
}