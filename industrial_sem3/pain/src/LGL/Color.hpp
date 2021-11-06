#ifndef LGL_COLOR_HPP
#define LGL_COLOR_HPP

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

        Color applyAlpha(float alpha) const
        {
            return Color(r, g, b, alpha);
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