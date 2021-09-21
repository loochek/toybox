#include <cassert>
#include "PhysicalWorld.hpp"

const int MAX_OBJECT_COUNT = 2000;

PhysicalWorld::PhysicalWorld(const Vec2f &worldSize): worldSize(worldSize)
{
    objects = new PhysicalCircle*[MAX_OBJECT_COUNT];
    objectsCount = 0;

    newVelocities = new Vec2f[MAX_OBJECT_COUNT];
}

PhysicalWorld::~PhysicalWorld()
{
    delete[] objects;
    delete[] newVelocities;
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

        object.update(elapsedTime);
        checkBounds(object);
    }

    handleCollisions();
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

void PhysicalWorld::handleCollisions()
{
    for (int i = 0; i < objectsCount; i++)
        newVelocities[i] = objects[i]->velocity;

    for (int i = 0; i < objectsCount; i++)
    {
        PhysicalCircle &obj1 = *objects[i];

        for (int j = i + 1; j < objectsCount; j++)
        {
            PhysicalCircle &obj2 = *objects[j];

            Vec2f intersectionPoint;
            if (!obj1.intersect(obj2, intersectionPoint))
                continue;

            Vec2f normal  = (obj1.position - intersectionPoint).normalized();
            Vec2f tangent = Vec2f(-normal.y, normal.x);

            Vec2f obj1VelNormProj = normal * (obj1.velocity ^ normal);
            Vec2f obj1VelTangProj = obj1.velocity - obj1VelNormProj;
            Vec2f obj2VelNormProj = normal * (obj2.velocity ^ normal);
            Vec2f obj2VelTangProj = obj2.velocity - obj2VelNormProj;

            newVelocities[i] += (obj2VelNormProj + obj1VelTangProj) - obj1.velocity;
            newVelocities[j] += (obj1VelNormProj + obj2VelTangProj) - obj2.velocity;
        }
    }

    for (int i = 0; i < objectsCount; i++)
        objects[i]->velocity = newVelocities[i];
}