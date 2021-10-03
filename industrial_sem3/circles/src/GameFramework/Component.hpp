#ifndef COMPONENT_HPP
#define COMPONENT_HPP

enum class Event
{
    PhysicalPositionChanged // PhysicalPositionChanged(const Vec2f *newPosition)
};

class Entity;

/**
 * Base abstract class for components
 */
class Component
{
public:
    /**
     * \return Entity which holds the component
     */
    Entity *getEntity() { return entity; };

protected:
    Component() = delete;
    Component(Entity *entity) : entity(entity) {};
    
    virtual ~Component() {};

    /// Component event handler
    friend class Entity;
    virtual void eventHandler(Event eventType, void *param1, void* param2) = 0;

protected:
    Entity *entity;
};

#endif