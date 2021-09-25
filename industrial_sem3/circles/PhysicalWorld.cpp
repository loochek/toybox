#include <cassert>
#include "PhysicalWorld.hpp"
#include "PhysicalCircle.hpp"

const int MAX_OBJECT_COUNT = 2000;

PhysicalWorld::PhysicalWorld(const Vec2f &worldSize): worldSize(worldSize)
{
    objects = new PhysicalObject*[MAX_OBJECT_COUNT];
    objectsCount = 0;

    newVelocities = new Vec2f[MAX_OBJECT_COUNT];
}

PhysicalWorld::~PhysicalWorld()
{
    delete[] objects;
    delete[] newVelocities;
}

void PhysicalWorld::addObject(PhysicalObject *object)
{
    assert(object != nullptr);

    if (objectsCount >= MAX_OBJECT_COUNT)
        return;

    objects[objectsCount++] = object;
}

void PhysicalWorld::update(float elapsedTime)
{
    for (int i = 0; i < objectsCount; i++)
    {
        PhysicalObject &object = *objects[i];

        object.update(elapsedTime);
        checkBounds(object);
    }

    for (int i = 0; i < objectsCount; i++)
    {
        PhysicalObject &obj1 = *objects[i];

        for (int j = i + 1; j < objectsCount; j++)
        {
            PhysicalObject &obj2 = *objects[j];

            IntersectFunc intersectFunc = PhysicalObject::intersectTable[(int)obj1.type][(int)obj2.type];
            CollideFunc   collideFunc   = PhysicalObject::collideTable  [(int)obj1.type][(int)obj2.type];

            Vec2f intersectionPoint;
            if (!intersectFunc(&obj1, &obj2, &intersectionPoint))
                continue;

            collideFunc(&obj1, &obj2);
        }
    }
}

void PhysicalWorld::checkBounds(PhysicalObject &object)
{
    PhysicalCircle *circle = (PhysicalCircle*)&object;

    // Top bound
    if (object.position.y - circle->radius < 0)
    {
        object.position.x += 2 * (circle->radius - object.position.y);
        object.velocity.y = -object.velocity.y;
    }

    // Left bound
    if (object.position.x - circle->radius < 0)
    {
        object.position.x += 2 * (circle->radius - object.position.x);
        object.velocity.x = -object.velocity.x;
    }

    // Bottom bound
    if (object.position.y + circle->radius > worldSize.y)
    {
        object.position.y -= 2 * (object.position.y + circle->radius - worldSize.y);
        object.velocity.y = -object.velocity.y;
    }

    // Right bound
    if (object.position.x + circle->radius > worldSize.x)
    {
        object.position.x -= 2 * (object.position.x + circle->radius - worldSize.x);
        object.velocity.x = -object.velocity.x;
    }
}