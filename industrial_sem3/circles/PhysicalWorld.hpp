#ifndef PHYSICAL_WORLD_HPP
#define PHYSICAL_WORLD_HPP

#include "PhysicalCircle.hpp"

/**
 * Abstract space with physics simulation
 */
class PhysicalWorld
{
public:
    PhysicalWorld(const Vec2f &worldSize = Vec2f(100.0f, 100.0f));
    ~PhysicalWorld();

    /**
     * Adds physical object to be controlled by the world
     * 
     * \param object Physical object
     */
    void addObject(PhysicalCircle *object);

    /**
     * Updates physical world to the specified time delta
     * 
     * \param elapsedTime Time delta
     */
    void update(float elapsedTime);

private:
    void checkBounds(PhysicalCircle &object);
    void handleCollisions();

    Vec2f worldSize;

    PhysicalCircle **objects;
    int objectsCount;

    Vec2f *newVelocities;
    bool  *objectUsed;
};

#endif