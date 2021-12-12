#ifndef PLUGIN_TYPES_PROXY_HPP
#define PLUGIN_TYPES_PROXY_HPP

#include "../EditorPluginAPI/plugin_std.hpp"
#include "../../Utils/Vec2.hpp"
#include "../../LGL/LGL.hpp"

template<typename T>
inline P::Vec2T<T> toPluginVec(Vec2<T> vec)
{
    return P::Vec2T<T>(vec.x, vec.y);
}

inline P::RGBA toPluginColor(const LGL::Color &color)
{
    return P::RGBA(color.r * EXTERNAL_RGB_BASE,
                   color.g * EXTERNAL_RGB_BASE,
                   color.b * EXTERNAL_RGB_BASE,
                   color.a * EXTERNAL_RGB_BASE);
}

template<typename T>
inline Vec2<T> fromPluginVec(P::Vec2T<T> vec)
{
    return Vec2<T>(vec.x, vec.y);
}

inline LGL::Color fromPluginColor(const P::RGBA &color)
{
    return LGL::Color(color.r / (float)EXTERNAL_RGB_BASE,
                      color.g / (float)EXTERNAL_RGB_BASE,
                      color.b / (float)EXTERNAL_RGB_BASE,
                      color.a / (float)EXTERNAL_RGB_BASE);
}

#endif