#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include "Entity.hpp"

class PhysicalObject;
class Drawable;

/**
 * Manager responsible for creation, storage, and destruction entities
 */
class EntityManager
{
public:
    EntityManager();
    ~EntityManager();

    Entity *createEntity();
    //void destroyEntity(Entity *entity);

private:
    Entity *entities;
    int entitiesCount;

    Drawable *drawableComponents;
    int drawableComponentsCount;

    PhysicalObject *physicalComponents;
    int physicalComponentsCount;
};

#endif