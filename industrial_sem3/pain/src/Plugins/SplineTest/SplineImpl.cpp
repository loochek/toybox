#include "../../Editor/AppInterface/PluginTypesProxy.hpp"
#include "SplineImpl.hpp"

const PUPPY::RGBA POINT_COLOR(255, 0, 0);
const Vec2i SPLINE_POINT_SIZE(5, 5);

const float SPLINE_THICKNESS = 1.0f;
const float SPLINE_STEP      = 10.0f;
const float POINT_SPAWN_DIST = 8.0f;

AbstractImpl::AbstractImpl(const PUPPY::WBody &body, const PUPPY::AppInterface *appInterface,
                           PUPPY::Widget *parent) :
    mBody(body), mParent(parent)
{
    mTexture = appInterface->factory.target->spawn(PUPPY::Vec2s(body.size.x, body.size.y));

    if (parent)
        parent->add_child(this);
}

SplinePoint::SplinePoint(const PUPPY::WBody &body, const PUPPY::AppInterface *appInterface, PUPPY::Widget *parent) :
    AbstractImpl(body, appInterface, parent)
{
}

void SplinePoint::on_render(const PUPPY::Event::Render &event)
{
    mTexture->clear(POINT_COLOR);
}

SplineImpl::SplineImpl(const PUPPY::WBody &body, const PUPPY::AppInterface *appInterface, PUPPY::Widget *parent) :
    AbstractImpl(body, appInterface, parent)
{
    PUPPY::Widget *point = new SplinePoint(PUPPY::WBody(PUPPY::Vec2f(10, 10), toPluginVec(Vec2f(SPLINE_POINT_SIZE))),
                              appInterface, this);
    //add_child(point);
    mPoints.push_back(point);
    recalcSpline();
}

void SplineImpl::on_render(const PUPPY::Event::Render &)
{
    mTexture->clear(PUPPY::RGBA(255, 255, 255));
        
    float maxParam = mSpline.getMaxParam();
    Vec2f prevPoint = mSpline[0];
    for (float t = 0; t <= maxParam; t += SPLINE_STEP)
    {
        Vec2f currPoint = mSpline[t];
        mTexture->render_circle(toPluginVec(currPoint), SPLINE_THICKNESS, PUPPY::RGBA(0, 0, 0));
        mTexture->render_line(toPluginVec(prevPoint), toPluginVec(currPoint), PUPPY::RGBA(0, 0, 0));
        prevPoint = currPoint;
    }

    // to avoid flickering with big step

    Vec2f currPoint = mSpline[maxParam];

    mTexture->render_circle(toPluginVec(currPoint), SPLINE_THICKNESS, PUPPY::RGBA(0, 0, 0));
    mTexture->render_line(toPluginVec(prevPoint), toPluginVec(currPoint), PUPPY::RGBA(0, 0, 0));
}

void SplineImpl::recalcSpline()
{
    std::sort(mPoints.begin(), mPoints.end(), [](PUPPY::Widget *first, PUPPY::Widget *second) -> bool
    {
        PUPPY::Vec2f thisPos  = first->get_body().position;
        PUPPY::Vec2f otherPos = second->get_body().position;

        if (thisPos.x == otherPos.x)
            return thisPos.y < otherPos.y;

        return thisPos.x < otherPos.x;
    });

    mSpline.clear();

    Vec2f diagVector = fromPluginVec(mBody.size).normalized();
    diagVector.y *= -1.0f;

    // First base point beyond bottom left corner of the widget
    mSpline.addPoint(Vec2f(0.0f, mBody.size.y) - diagVector);

    // First visible point at bottom left corner of the widget
    mSpline.addPoint(Vec2f(0.0f, mBody.size.y));

    // User-defined points
    for (PUPPY::Widget *child : mPoints)
    {
        SplinePoint *point = static_cast<SplinePoint*>(child);

        if (!IntRect(Vec2i(), fromPluginVec(mBody.size)).contains(fromPluginVec(point->get_body().position)))
        {
            // Remove point if it goes outside
            point->set_to_delete();
            continue;
        }

        mSpline.addPoint(fromPluginVec(point->get_body().position) + SPLINE_POINT_SIZE / 2);
    }

    // Second visible point at top right corner of the widget
    mSpline.addPoint(Vec2f(mBody.size.x, 0.0f));

    // Second base point beyond bottom left corner of the widget
    mSpline.addPoint(Vec2f(mBody.size.x, 0.0f) + diagVector);
}