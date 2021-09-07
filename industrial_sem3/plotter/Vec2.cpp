#include <math.h>
#include "Vec2.hpp"

Vec2 Vec2::operator+(const Vec2 &other)
{
    return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator-()
{
    return Vec2(-x, -y);
}

Vec2 Vec2::operator-(const Vec2 &other)
{
    return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator*(double multiplier)
{
    return Vec2(x * multiplier, y * multiplier);
}

Vec2 Vec2::operator/(double divider)
{
    return Vec2(x / divider, y / divider);
}

Vec2 &Vec2::operator+=(const Vec2 &other)
{
    x += other.x;
    y += other.y;

    return *this;
}

Vec2 &Vec2::operator-=(const Vec2 &other)
{
    x -= other.x;
    y -= other.y;

    return *this;
}

Vec2 &Vec2::operator*=(double multiplier)
{
    x *= multiplier;
    y *= multiplier;

    return *this;
}

Vec2 &Vec2::operator/=(double divider)
{
    x /= divider;
    y /= divider;

    return *this;
}

double Vec2::length()
{
    return sqrt(x * x + y * y);
}

void Vec2::normalize()
{
    if (x == 0 && y == 0)
        return;

    double len = length();
    x /= len;
    y /= len;
}

sf::Vector2f Vec2::toSFMLVector()
{
    return sf::Vector2f(x, y);
}