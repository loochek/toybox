#ifndef LGL_SFML_COMMON_HPP
#define LGL_SFML_COMMON_HPP

#include <SFML/Graphics.hpp>
#include "../../Math/Vec2.hpp"
#include "../../Math/Vec3.hpp"
#include "../../Math/Rect.hpp"

namespace LGL
{
    typedef Vec3f Color;

    enum class TextOrigin
    {
        Default,
        Centered
    };

    inline sf::Vector2f toSFMLVector(const Vec2f &vector)
    {
        return sf::Vector2f(vector.x, vector.y);
    }

    inline sf::Color toSFMLColor(const Color &color)
    {
        return sf::Color(color.x * 255, color.y * 255, color.z * 255);
    }

    inline Vec2f fromSFMLVector(const sf::Vector2f &vector)
    {
        return Vec2f(vector.x, vector.y);
    }

    inline Vec2i fromSFMLVector(const sf::Vector2u &vector)
    {
        return Vec2f(vector.x, vector.y);
    }
};

#endif