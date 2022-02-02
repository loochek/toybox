#ifndef LGL_COLOR_HPP
#define LGL_COLOR_HPP

#include <algorithm>

const int EXTERNAL_RGB_BASE = 255;

namespace LGL
{
    class Color
    {
    public:
        Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {};
        Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {};

        Color operator+(const Color &other) const
        {
            return Color(r + other.r, g + other.g, b + other.b, a + other.a);
        }

        Color operator-(const Color &other) const
        {
            return Color(r - other.r, g - other.g, b - other.b, a - other.a);
        }

        Color operator*(float multiplier) const
        {
            return Color(r * multiplier, g * multiplier, b * multiplier, a * multiplier);
        }

        Color operator/(float divider) const
        {
            return Color(r / divider, g / divider, b / divider, a / divider);
        }

        Color &operator+=(const Color &other)
        {
            r += other.r;
            g += other.g;
            b += other.b;
            a += other.a;
            return *this;
        }

        Color &operator-=(const Color &other)
        {
            r -= other.r;
            g -= other.g;
            b -= other.b;
            a -= other.a;
            return *this;
        }

        Color &operator*=(float multiplier)
        {
            r *= multiplier;
            g *= multiplier;
            b *= multiplier;
            a *= multiplier;
            return *this;
        }

        Color &operator/=(float divider)
        {
            r /= divider;
            g /= divider;
            b /= divider;
            a /= divider;
            return *this;
        }

        Color applyAlpha(float alpha) const
        {
            return Color(r, g, b, alpha);
        }

        Color norm() const
        {
            return Color(std::max(0.0f, std::min(r, 1.0f)),
                         std::max(0.0f, std::min(g, 1.0f)),
                         std::max(0.0f, std::min(b, 1.0f)),
                         std::max(0.0f, std::min(a, 1.0f)));
        }

    public:
        float r;
        float g;
        float b;
        float a;

        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Yellow;
        static const Color Magenta;
        static const Color Cyan;
        static const Color Pink;
        static const Color Brown;
        static const Color Gray;
        static const Color Transparent;
    };
};

#endif