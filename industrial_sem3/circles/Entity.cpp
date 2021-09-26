 #include <cassert>
 #include "Entity.hpp"

Entity::~Entity()
{
    if (drawableComponentPresent)
        delete drawableComponent;
    if (physicalComponentPresent)
        delete physicalComponent;
 }

void Entity::addDrawableComponent(Drawable *drawable)
{
    assert(drawable != nullptr);

    drawableComponent = drawable;
    drawableComponentPresent = true;
}

void Entity::addPhysicalComponent(PhysicalObject *physicalObject)
{
    assert(physicalObject != nullptr);

    physicalComponent = physicalObject;
    physicalComponentPresent = true;
}