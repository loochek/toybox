 #include <cassert>
 #include "Entity.hpp"

Entity::Entity(App *engine) : engine(engine),
                              drawableComponentPresent(false), drawableComponent(nullptr),
                              physicalComponentPresent(false), physicalComponent(nullptr),
                              scheduledForDeletion(false), active(true)
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

void Entity::scheduleForDeletion()
{
    active = false;
    scheduledForDeletion = true;
}

void Entity::enable()
{
    active = true;
}

void Entity::disable()
{
    active = false;
}