#ifndef VEC2F_HPP
#define VEC2F_HPP

#include <SFML/System.hpp>

/**
 * Two-component vector
 */
class Vec2
{
public:
    Vec2() : x(0), y(0) {};
    Vec2(double x, double y) : x(x), y(y) {};
    Vec2(const sf::Vector2f &sfVec) : x(sfVec.x), y(sfVec.y) {};

    Vec2 operator+(const Vec2 &other);
    Vec2 operator-();
    Vec2 operator-(const Vec2 &other);
    Vec2 operator*(double multiplier);
    Vec2 operator/(double divider);

    Vec2 &operator+=(const Vec2 &other);
    Vec2 &operator-=(const Vec2 &other);
    Vec2 &operator*=(double multiplier);
    Vec2 &operator/=(double divider);

    double length();
    void normalize();

    sf::Vector2f toSFMLVector();
    
    double x;
    double y;
};

#endif