 #include <cassert>
 #include "Entity.hpp"

Entity::Entity() : drawableComponentPresent(false), drawableComponent(nullptr),
                   physicalComponentPresent(false), physicalComponent(nullptr),
                   scheduledForDeletion(false)
{
}

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

void Entity::sendEvent(Event eventType, void *param1, void* param2)
{
    if (drawableComponentPresent)
        drawableComponent->eventHandler(eventType, param1, param2);

    if (physicalComponentPresent)
        physicalComponent->eventHandler(eventType, param1, param2);
}