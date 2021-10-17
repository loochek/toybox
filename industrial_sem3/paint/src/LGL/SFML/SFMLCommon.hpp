#ifndef LGL_SFML_COMMON_HPP
#define LGL_SFML_COMMON_HPP

#include <SFML/Graphics.hpp>
#include "../../Math/Vec2.hpp"
#include "../../Math/Vec3.hpp"
#include "../../Math/Rect.hpp"
#include "../Color.hpp"

namespace LGL
{
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
        return sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
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