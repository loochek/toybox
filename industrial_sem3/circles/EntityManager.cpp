#include "EntityManager.hpp"
#include "Drawable.hpp"
#include "PhysicalObject.hpp"

const int MAX_ENTITIES_COUNT = 100;

EntityManager::EntityManager()
{
    entities = new Entity[MAX_ENTITIES_COUNT];
    drawableComponents = new Drawable[MAX_ENTITIES_COUNT];
}

EntityManager::~EntityManager()
{
    delete[] entities;
}

Entity *EntityManager::createEntity()
{
    if (entitiesCount >= MAX_ENTITIES_COUNT)
        return nullptr;

    return &entities[entitiesCount++];
}