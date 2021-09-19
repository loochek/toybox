#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

class Graphics;

/**
 * Abstract class for objects that can be drawn
 */
class Drawable
{
    friend class Graphics;

protected:
    virtual void draw(Graphics &graphics) const = 0;
};

#endif