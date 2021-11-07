#include <algorithm>
#include <math.h>
#include "CatmullRomSpline.hpp"

const float ALPHA = 1;

CatmullRomSpline::CatmullRomSpline()
{
}

void CatmullRomSpline::addPoint(const Vec2f &point)
{
    if (mPoints.size() == 0)
        mParams.push_back(0);
    else
    {
        float lastParam = mParams[mParams.size() - 1];
        Vec2f lastPoint = mPoints[mPoints.size() - 1];
        mParams.push_back(getNextParam(lastParam, lastPoint, point));
    }

    mPoints.push_back(point);
}

Vec2f CatmullRomSpline::operator[](float t)
{
    if (t < 0.0f || t > getMaxParam() || mParams.size() < 4)
        return Vec2f(0.0f, 0.0f);

    // Determine segment

    t += mParams[1];

    int i2 = std::upper_bound(mParams.begin(), mParams.end(), t) - mParams.begin();
    int i0 = i2 - 2;
    int i1 = i2 - 1;
    int i3 = i2 + 1;

    float t0 = mParams[i0];
    float t1 = mParams[i1] - t0;
    float t2 = mParams[i2] - t0;
    float t3 = mParams[i3] - t0;
    t -= t0;
    t0 = 0.0f;

    Vec2f p0 = mPoints[i0];
    Vec2f p1 = mPoints[i1];
    Vec2f p2 = mPoints[i2];
    Vec2f p3 = mPoints[i3];

    // Usual Catmull-Rom formulas

    Vec2f a1 = p0 * ((t1 - t) / (t1 - t0)) + p1 * ((t - t0) / (t1 - t0));
    Vec2f a2 = p1 * ((t2 - t) / (t2 - t1)) + p2 * ((t - t1) / (t2 - t1));
    Vec2f a3 = p2 * ((t3 - t) / (t3 - t2)) + p3 * ((t - t2) / (t3 - t2));

    Vec2f b1 = a1 * ((t2 - t) / (t2 - t0)) + a2 * ((t - t0) / (t2 - t0));
    Vec2f b2 = a2 * ((t3 - t) / (t3 - t1)) + a3 * ((t - t1) / (t3 - t1));

    return b1 * ((t2 - t) / (t2 - t1)) + b2 * ((t - t1) / (t2 - t1));
}

float CatmullRomSpline::getMaxParam()
{
    if (mParams.size() < 4)
        return 0.0f;

    return mParams[mParams.size() - 2] - mParams[1];
}

float CatmullRomSpline::getNextParam(float t, const Vec2f &p1, const Vec2f &p2)
{
    return t + pow((p2 - p1).length(), ALPHA);
}