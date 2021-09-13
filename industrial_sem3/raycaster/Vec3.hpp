#ifndef VEC3_HPP
#define VEC3_HPP

#include <math.h>

/**
 * Three-component vector
 */
template<typename T>
class Vec3
{
public:
    /// Initializes a zero vector
    Vec3() : x(0), y(0), z(0) {};
    Vec3(T x, T y, T z) : x(x), y(y), z(z) {};

    Vec3 operator+(const Vec3 &other) const
    {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-() const
    {
        return Vec3(-x, -y, -z);
    }

    Vec3 operator-(const Vec3 &other) const
    {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(T multiplier) const
    {
        return Vec3(x * multiplier, y * multiplier, z * multiplier);
    }

    Vec3 operator/(T divider) const
    {
        return Vec3(x / divider, y / divider, z / divider);
    }

    // Per-component multiplication
    Vec3 operator*(const Vec3 &other) const
    {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }

    // Per-component division
    Vec3 operator/(const Vec3 &other) const
    {
        return Vec3(x / other.x, y / other.y, z / other.z);
    }

    /// Scalar multiplication
    T operator^(const Vec3 &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    /// Vector multiplication
    Vec3 operator&(const Vec3 &other) const
    {
        return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    Vec3 normalized() const
    {
        if (x == 0 && y == 0 && z == 0)
            return Vec3();

        double len = length();
        return *this / len;
    }

    T length() const
    {
        return sqrt(*this ^ *this);
    }

    Vec3 reflected(Vec3 normal) const
    {
        return *this - normal * (2 * (*this ^ normal));
    }

    Vec3 &operator+=(const Vec3 &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;

        return *this;
    }

    Vec3 &operator-=(const Vec3 &other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;

        return *this;
    }

    Vec3 &operator*=(T multiplier)
    {
        x *= multiplier;
        y *= multiplier;
        z *= multiplier;

        return *this;
    }

    Vec3 &operator/=(T divider)
    {
        x /= divider;
        y /= divider;
        z /= divider;

        return *this;
    }
    
    T x;
    T y;
    T z;
};

/// Three-component float vector
typedef Vec3<float> Vec3f;

#endif