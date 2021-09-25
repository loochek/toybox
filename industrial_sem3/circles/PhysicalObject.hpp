#ifndef PHYSICAL_OBJECT_HPP
#define PHYSICAL_OBJECT_HPP

#include "Vec2.hpp"

const int PHYS_OBJ_TYPE_COUNT = 3;

enum class PhysicalObjectType
{
    Invalid,
    Circle,
    Circle2,
};

class PhysicalObject;

typedef bool (*IntersectFunc)(const PhysicalObject *obj1, const PhysicalObject *obj2, Vec2f *intersectionPoint);
typedef void (*CollideFunc)(PhysicalObject *obj1, PhysicalObject *obj2);


/**
 * Base class for a physical object
 */
class PhysicalObject
{
public:
    PhysicalObject(PhysicalObjectType type = PhysicalObjectType::Invalid,
                   const Vec2f &position = Vec2f(), float mass = 1.0f, const Vec2f &velocity = Vec2f()) :
                   type(type), position(position), mass(mass), velocity(velocity) {};

    /**
     * Updates object position to the specified time delta
     * 
     * \param elapsedTime Time delta
     */
    virtual void update(float elapsedTime);

public:
    Vec2f position;
    Vec2f velocity;

    float mass;

    PhysicalObjectType type;

    static const IntersectFunc intersectTable[PHYS_OBJ_TYPE_COUNT][PHYS_OBJ_TYPE_COUNT];
    static const CollideFunc   collideTable  [PHYS_OBJ_TYPE_COUNT][PHYS_OBJ_TYPE_COUNT];
};

#endif