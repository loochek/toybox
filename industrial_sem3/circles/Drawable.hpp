#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "Graphics.h"

/**
 * Abstract class for objects that can be drawn
 */
class Drawable
{
public:
    virtual void draw(Graphics &graphics) = 0;
};

#endif