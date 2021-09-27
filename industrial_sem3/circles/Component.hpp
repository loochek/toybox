#ifndef COMPONENT_HPP
#define COMPONENT_HPP

enum class Event
{
    PhysicalPositionChanged,      // PhysicalPositionChanged(const Vec2f *newPosition)
    CollisionOccured,             // CollisionOccured(Entity *other)
    CircleHeavyCollisionOccured,  // CircleHeavyCollisionOccured (Entity *other)
    Circle2HeavyCollisionOccured, // Circle2HeavyCollisionOccured(Entity *other)
};

class Entity;

/**
 * Base abstract class for components
 */
class Component
{
protected:
    Component() = delete;
    Component(Entity *entity) : entity(entity) {};
    
    virtual ~Component() {};

    /// Component event handler
    friend class Entity;
    virtual void eventHandler(Event eventType, void *param1, void* param2) = 0;

public:
    Entity *entity;
};

#endif