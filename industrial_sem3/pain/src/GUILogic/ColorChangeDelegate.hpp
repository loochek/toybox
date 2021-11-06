#ifndef COLOR_CHANGE_DELEGATE_HPP
#define COLOR_CHANGE_DELEGATE_HPP

#include "../LGL/LGL.hpp"

/**
 * Used by color pickers to notify about color change
 */
class ColorChangeDelegate
{
public:
    virtual ~ColorChangeDelegate() {};

    virtual void onColorChange(const LGL::Color &color, int userData) = 0;
};

#endif