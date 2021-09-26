#include <cassert>
#include "PhysicalSystem.hpp"
#include "Entity.hpp"

const int MAX_OBJECTS_COUNT = 100;

PhysicalSystem::PhysicalSystem()
{
    components = new PhysicalObject*[MAX_OBJECTS_COUNT];
    componentsCount = 0;
}

PhysicalSystem::~PhysicalSystem()
{
    delete[] components;
}

void PhysicalSystem::registerComponent(PhysicalObject *component)
{
    assert(component != nullptr);

    if (componentsCount >= MAX_OBJECTS_COUNT)
        return;

    components[componentsCount++] = component;
}

void PhysicalSystem::update(float elapsedTime)
{
    // Update components
    for (int i = 0; i < componentsCount; i++)
    {
        PhysicalObject &obj = *components[i];
        obj.update(elapsedTime);
        obj.entity->sendEvent(Event::PhysicalPositionChanged, &obj.position, nullptr);
    }

    // Handle collisions
    for (int i = 0; i < componentsCount; i++)
    {
        PhysicalObject &obj1 = *components[i];

        for (int j = i + 1; j < componentsCount; j++)
        {
            PhysicalObject &obj2 = *components[j];

            IntersectFunc intersectFunc = PhysicalObject::intersectTable[(int)obj1.type][(int)obj2.type];
            CollideFunc   collideFunc   = PhysicalObject::collideTable  [(int)obj1.type][(int)obj2.type];

            if (!intersectFunc(&obj1, &obj2))
                continue;

            collideFunc(&obj1, &obj2);
        }
    }
}