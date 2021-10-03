 #include <cassert>
 #include "Entity.hpp"

Entity::Entity(GameFramework *engine) : engine(engine),
                                        drawableComponent(nullptr),
                                        physicalComponent(nullptr),
                                        scheduledForDeletion(false), active(true)
{
}

Entity::~Entity()
{
    if (drawableComponent != nullptr)
        delete drawableComponent;
    if (physicalComponent != nullptr)
        delete physicalComponent;
 }

void Entity::addDrawableComponent(Drawable *drawable)
{
    assert(drawable != nullptr);

    drawableComponent = drawable;
}

void Entity::addPhysicalComponent(PhysicalObject *physicalObject)
{
    assert(physicalObject != nullptr);

    physicalComponent = physicalObject;
}

void Entity::sendEvent(Event eventType, void *param1, void* param2)
{
    if (drawableComponent != nullptr)
        drawableComponent->eventHandler(eventType, param1, param2);

    if (physicalComponent != nullptr)
        physicalComponent->eventHandler(eventType, param1, param2);
}