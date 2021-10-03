#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "../Math/Rect.hpp"
#include "../GameFramework/Component.hpp"

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
    virtual void draw(Graphics &graphics, const Rect &viewport) = 0;
};

#endif