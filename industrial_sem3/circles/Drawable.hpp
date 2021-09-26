#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "Component.hpp"

class Graphics;

/**
 * Abstract component for objects that can be drawn
 */
class Drawable : public Component
{
protected:
    Drawable(Entity *entity) : Component(entity) {};
    
    // to access draw method
    friend class RenderSystem;
    virtual void draw(Graphics &graphics) = 0;
};

#endif