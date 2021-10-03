#ifndef PHYSICAL_OBJECT_HPP
#define PHYSICAL_OBJECT_HPP

#include "../GameFramework/Component.hpp"
#include "../Math/Vec2.hpp"

const int PHYS_OBJ_TYPE_COUNT = 4;

enum class PhysicalObjectType
{
    Invalid,
    Bound,
    Circle,
    Circle2,
};

class PhysicalObject;

// Function declarations for the interaction of different types of physical objects with each other

typedef bool (*IntersectFunc)(const PhysicalObject *obj1, const PhysicalObject *obj2);
typedef void (*CollideFunc)(PhysicalObject *obj1, PhysicalObject *obj2);

/**
 * Base class for a physical component
 */
class PhysicalObject : public Component
{
public:
    /**
     * Updates object position to the specified time delta
     * 
     * \param elapsedTime Time delta
     */
    virtual void update(float elapsedTime);

protected:
    PhysicalObject(Entity *entity, PhysicalObjectType type = PhysicalObjectType::Invalid,
                   const Vec2f &position = Vec2f(), const Vec2f &velocity = Vec2f()) :
                   Component(entity), type(type), position(position), velocity(velocity) {};

    /// Component event handler
    friend class Entity;
    virtual void eventHandler(Event eventType, void *param1, void* param2);

public:
    Vec2f position;
    Vec2f velocity;

    static const IntersectFunc intersectTable[PHYS_OBJ_TYPE_COUNT][PHYS_OBJ_TYPE_COUNT];
    static const CollideFunc   collideTable  [PHYS_OBJ_TYPE_COUNT][PHYS_OBJ_TYPE_COUNT];

protected:
    // to access a physical object type
    friend class PhysicalSystem;

    PhysicalObjectType type;
};

#endif