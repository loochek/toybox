#ifndef VEC2_HPP
#define VEC2_HPP

#include <math.h>

/**
 * Two-component vector
 */
template<typename T>
class Vec2
{
public:
    /// Initializes a zero vector
    Vec2() : x(0), y(0) {};
    Vec2(T x, T y) : x(x), y(y) {};

    /// Convert constructor
    template <typename U>
    Vec2(const Vec2<U> &other) : x(other.x), y(other.y) {};

    bool operator==(const Vec2 &other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vec2 &other) const
    {
        return x != other.x || y != other.y;
    }

    Vec2 operator+(const Vec2 &other) const
    {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 operator-() const
    {
        return Vec2(-x, -y);
    }

    Vec2 operator-(const Vec2 &other) const
    {
        return Vec2(x - other.x, y - other.y);
    }

    Vec2 operator*(T multiplier) const
    {
        return Vec2(x * multiplier, y * multiplier);
    }

    Vec2 operator/(T divider) const
    {
        return Vec2(x / divider, y / divider);
    }

    // Per-component multiplication
    Vec2 operator*(const Vec2 &other) const
    {
        return Vec2(x * other.x, y * other.y);
    }

    // Per-component division
    Vec2 operator/(const Vec2 &other) const
    {
        return Vec2(x / other.x, y / other.y);
    }

    /// Scalar multiplication
    T operator^(const Vec2 &other) const
    {
        return x * other.x + y * other.y;
    }

    Vec2 normalized() const
    {
        if (x == 0 && y == 0)
            return Vec2();

        double len = length();
        return *this / len;
    }

    T length() const
    {
        return sqrt(*this ^ *this);
    }

    Vec2 reflected(Vec2 normal) const
    {
        return *this - normal * (2 * (*this ^ normal));
    }

    Vec2 rotated(float angle) const
    {
        float s = sin(angle), c = cos(angle);

        return Vec2(x * c - y * s, x * s + y * c);
    }

    Vec2 &operator+=(const Vec2 &other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    Vec2 &operator-=(const Vec2 &other)
    {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    Vec2 &operator*=(T multiplier)
    {
        x *= multiplier;
        y *= multiplier;

        return *this;
    }

    Vec2 &operator/=(T divider)
    {
        x /= divider;
        y /= divider;

        return *this;
    }
    
    T x;
    T y;
};

/// Two-component float vector
typedef Vec2<float> Vec2f;

/// Two-component int vector
typedef Vec2<int> Vec2i;

#endif