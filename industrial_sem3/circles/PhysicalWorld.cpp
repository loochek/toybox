#include <cassert>
#include "PhysicalWorld.hpp"

const int MAX_OBJECT_COUNT = 100;

PhysicalWorld::PhysicalWorld(const Vec2f &worldSize): worldSize(worldSize)
{
    objects = new PhysicalCircle*[MAX_OBJECT_COUNT];
    objectsCount = 0;
}

PhysicalWorld::~PhysicalWorld()
{
    delete[] objects;
}

void PhysicalWorld::addObject(PhysicalCircle *object)
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
        PhysicalCircle &object = *objects[i];

        object.position += object.velocity * elapsedTime;
        checkBounds(object);
    }
}

void PhysicalWorld::checkBounds(PhysicalCircle &object)
{
    // Top bound
    if (object.position.y - object.radius < 0)
    {
        object.position.x += 2 * (object.radius - object.position.y);
        object.velocity.y = -object.velocity.y;
    }

    // Left bound
    if (object.position.x - object.radius < 0)
    {
        object.position.x += 2 * (object.radius - object.position.x);
        object.velocity.x = -object.velocity.x;
    }

    // Bottom bound
    if (object.position.y + object.radius > worldSize.y)
    {
        object.position.y -= 2 * (object.position.y + object.radius - worldSize.y);
        object.velocity.y = -object.velocity.y;
    }

    // Right bound
    if (object.position.x + object.radius > worldSize.x)
    {
        object.position.x -= 2 * (object.position.x + object.radius - worldSize.x);
        object.velocity.x = -object.velocity.x;
    }
}