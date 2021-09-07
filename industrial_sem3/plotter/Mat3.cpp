#include <cstring>
#include <math.h>
#include "Mat3.hpp"

Mat3::Mat3()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            data[i][j] = 0;
}

Mat3::Mat3(const double (&data)[3][3])
{
    memcpy(this->data, data, 3 * 3 * sizeof(double));
}

Mat3 Mat3::zeroMatrix()
{
    return Mat3({ { 0, 0, 0 },
                  { 0, 0, 0 },
                  { 0, 0, 0 } });
}

Mat3 Mat3::unitMatrix()
{
    return Mat3({ { 1, 0, 0 },
                  { 0, 1, 0 },
                  { 0, 0, 1 } });
}

Mat3 Mat3::translationMatrix(double xOffs, double yOffs)
{
    return Mat3({ { 1, 0, xOffs },
                  { 0, 1, yOffs },
                  { 0, 0, 1 } });
}

Mat3 Mat3::rotationMatrix(double angle)
{
    double s = sin(angle), c = cos(angle);

    return Mat3({ { c, -s, 0 },
                  { s,  c, 0 },
                  { 0,  0, 1 } });
}

Mat3 Mat3::scaleMatrix(double xScale, double yScale)
{
    return Mat3({ { xScale, 0     , 0 },
                  { 0     , yScale, 0 },
                  { 0     , 0     , 1 } });
}

Mat3 Mat3::operator+(const Mat3 &other)
{
    Mat3 result;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            result.data[i][j] = data[i][j] + other.data[i][j];

    return result;
}

Mat3 Mat3::operator-()
{
    Mat3 result;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            result.data[i][j] = -data[i][j];

    return result;
}

Mat3 Mat3::operator-(const Mat3 &other)
{
    Mat3 result;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            result.data[i][j] = data[i][j] - other.data[i][j];

    return result;
}

Mat3 Mat3::operator*(double multiplier)
{
    Mat3 result;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            result.data[i][j] = data[i][j] * multiplier;

    return result;
}

Mat3 Mat3::operator/(double divider)
{
    Mat3 result;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            result.data[i][j] = data[i][j] / divider;

    return result;
}

Mat3 &Mat3::operator+=(const Mat3 &other)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            data[i][j] += other.data[i][j];

    return *this;
}

Mat3 &Mat3::operator-=(const Mat3 &other)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            data[i][j] -= other.data[i][j];

    return *this;
}

Mat3 &Mat3::operator*=(double multiplier)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            data[i][j] *= multiplier;

    return *this;
}

Mat3 &Mat3::operator/=(double divider)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            data[i][j] /= divider;

    return *this;
}

Mat3 Mat3::operator*(const Mat3 &other)
{
    Mat3 result;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                result.data[i][j] += data[i][k] * other.data[k][j];

    return result;
}

Vec2 Mat3::operator*(const Vec2 &vec)
{
    return Vec2(data[0][0] * vec.x + data[0][1] * vec.y + data[0][2],
                data[1][0] * vec.x + data[1][1] * vec.y + data[1][2]);
}