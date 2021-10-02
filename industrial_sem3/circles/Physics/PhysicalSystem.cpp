#include <cassert>
#include "PhysicalSystem.hpp"
#include "../Entity.hpp"

PhysicalSystem::PhysicalSystem()
{
}

PhysicalSystem::~PhysicalSystem()
{
}

void PhysicalSystem::registerComponent(PhysicalObject *component)
{
    assert(component != nullptr);

    components.insert(component);
}

void PhysicalSystem::unregisterComponent(PhysicalObject *component)
{
    assert(component != nullptr);

    components.erase(component);
}

void PhysicalSystem::update(float elapsedTime)
{
    // Update components
    for (PhysicalObject *obj : components)
    {
        if (!obj->entity->active)
            continue;

        obj->update(elapsedTime);
        obj->entity->sendEvent(Event::PhysicalPositionChanged, &obj->position, nullptr);
    }

    // Handle collisions
    for (auto obj1 = components.begin(); obj1 != components.end(); obj1++)
    {
        if (!(*obj1)->entity->active)
            continue;

        for (auto obj2 = obj1; ++obj2 != components.end();)
        {
            // Object can be disabled between iterations
            if (!(*obj1)->entity->active)
                break;

            if (!(*obj2)->entity->active)
                continue;

            IntersectFunc intersectFunc = PhysicalObject::intersectTable[(int)(*obj1)->type][(int)(*obj2)->type];
            CollideFunc   collideFunc   = PhysicalObject::collideTable  [(int)(*obj1)->type][(int)(*obj2)->type];

            if (!intersectFunc(*obj1, *obj2))
                continue;

            collideFunc(*obj1, *obj2);
        }
    }
}