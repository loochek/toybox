#include <cassert>
#include "PhysicalSystem.hpp"
#include "Entity.hpp"

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
        obj->update(elapsedTime);
        obj->entity->sendEvent(Event::PhysicalPositionChanged, &obj->position, nullptr);
    }

    // Handle collisions
    for (auto obj1 = components.begin(); obj1 != components.end(); obj1++)
    {
        for (auto obj2 = obj1; ++obj2 != components.end();)
        {
            IntersectFunc intersectFunc = PhysicalObject::intersectTable[(int)(*obj1)->type][(int)(*obj2)->type];
            CollideFunc   collideFunc   = PhysicalObject::collideTable  [(int)(*obj1)->type][(int)(*obj2)->type];

            if (!intersectFunc(*obj1, *obj2))
                continue;

            collideFunc(*obj1, *obj2);
        }
    }
}