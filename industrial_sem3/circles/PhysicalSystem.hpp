#ifndef PHYSICAL_SYSTEM_HPP
#define PHYSICAL_SYSTEM_HPP

#include "PhysicalObject.hpp"

/**
 * Controls physical components of the entities, representing an abstract physical space
 */
class PhysicalSystem
{
public:
    PhysicalSystem();
    ~PhysicalSystem();

    /**
     * Adds physical component to be controlled
     * 
     * \param component Component
     */
    void registerComponent(PhysicalObject *component);

    /**
     * Updates physical space to the specified time delta
     * 
     * \param elapsedTime Time delta
     */
    void update(float elapsedTime);

private:
    PhysicalObject **components;
    int componentsCount;
};

#endif