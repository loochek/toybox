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

    Vec3 operator+(const Vec3 &other)
    {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-()
    {
        return Vec3(-x, -y, -z);
    }

    Vec3 operator-(const Vec3 &other)
    {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(T multiplier)
    {
        return Vec3(x * multiplier, y * multiplier, z * multiplier);
    }

    Vec3 operator/(T divider)
    {
        return Vec3(x / divider, y / divider, z / divider);
    }

    // Per-component multiplication
    Vec3 operator*(const Vec3 &other)
    {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }

    // Per-component division
    Vec3 operator/(const Vec3 &other)
    {
        return Vec3(x / other.x, y / other.y, z / other.z);
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

    /// Scalar multiplication
    T operator^(const Vec3 &other)
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 normalized()
    {
        if (x == 0 && y == 0 && z == 0)
            return Vec3();

        double len = length();
        return *this / len;
    }

    T length()
    {
        return sqrt(*this ^ *this);
    }

    Vec3 reflected(Vec3 normal)
    {
        return *this - normal * (2 * (*this ^ normal));
    }
    
    T x;
    T y;
    T z;
};

/// Three-component float vector
typedef Vec3<float> Vec3f;

#endif