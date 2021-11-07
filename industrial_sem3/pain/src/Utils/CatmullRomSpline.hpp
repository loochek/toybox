#ifndef CATMULL_ROM_SPLINE_HPP
#define CATMULL_ROM_SPLINE_HPP

#include <vector>
#include "Vec2.hpp"

/**
 * Catmull-Rom spline generator. Behaves like a parameterized curve
 */
class CatmullRomSpline
{
public:
    CatmullRomSpline();

    /**
     * Extends the spline with a new point. 
     * At least 4 points must be added for spline generation.
     * 
     * \param point Point
     */
    void addPoint(const Vec2f &point);

    /**
     * \return Spline point according to parameter
     */
    Vec2f operator[](float t);

    float getMaxParam();

private:
    float getNextParam(float t, const Vec2f &p1, const Vec2f &p2);

private:
    std::vector<Vec2f> mPoints;
    std::vector<float> mParams;
};

#endif