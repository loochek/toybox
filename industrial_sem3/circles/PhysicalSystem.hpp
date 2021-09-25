#ifndef PHYSICAL_SYSTEM_HPP
#define PHYSICAL_SYSTEM_HPP

#include "EntityManager.hpp"

/**
 * Controls physical components of the entities, representing an abstract physical space
 */
class PhysicalSystem
{
public:
    PhysicalSystem() = delete;
    PhysicalSystem(EntityManager *entityMgr) : entityMgr(entityMgr) {};

    /**
     * Updates physical space to the specified time delta
     * 
     * \param elapsedTime Time delta
     */
    void update(float elapsedTime);

private:
    EntityManager *entityMgr;
};

#endif