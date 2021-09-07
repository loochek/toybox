#ifndef MAT3_HPP
#define MAT3_HPP

#include "Vec2.hpp"

/**
 * 3x3 matrix
 */
class Mat3
{
public:
    /// Initializes matrix with zeroes
    Mat3();
    Mat3(const double (&data)[3][3]);

    static Mat3 zeroMatrix();
    static Mat3 unitMatrix();

    static Mat3 translationMatrix(double xOffs, double yOffs);
    static Mat3 rotationMatrix   (double angle);
    static Mat3 scaleMatrix      (double xScale, double yScale);

    Mat3 operator+(const Mat3 &other);
    Mat3 operator-();
    Mat3 operator-(const Mat3 &other);
    Mat3 operator*(double multiplier);
    Mat3 operator/(double divider);

    Mat3 &operator+=(const Mat3 &other);
    Mat3 &operator-=(const Mat3 &other);
    Mat3 &operator*=(double multiplier);
    Mat3 &operator/=(double divider);

    Mat3 operator*(const Mat3 &other);

    /// Applies transform to the vector
    Vec2 operator*(const Vec2 &vec);

    double data[3][3];
};

#endif