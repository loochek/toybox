#include "GameFramework.hpp"
#include "../Physics/PhysicalCircle.hpp"
#include "../Physics/PhysicalCircle2.hpp"
#include "../Render/DrawableCircle.hpp"
#include "../Render/DrawableSquare.hpp"

GameFramework::GameFramework(Graphics &graphics, const Rect &viewport) : renderSystem(graphics, viewport)
{
}

GameFramework::~GameFramework()
{
    for (Entity *entity : entities)
    {
        delete entity;
    }
}

void GameFramework::gameUpdate(float elapsedTime)
{
    physicalSystem.update(elapsedTime);
 
    deleteEntities();
    for (Entity *ent : entities)
        ent->enable();
}

void GameFramework::gameDraw()
{
    renderSystem.draw();
}

void GameFramework::addEntity(Entity *entity)
{
    entities.insert(entity);
}

Entity *GameFramework::createCircle(Vec2f position, float radius, Color color, Vec2f velocity)
{
    Entity *ent = new Entity(this);

    Drawable *drawable = new DrawableCircle(ent, position, radius, color);
    ent->addDrawableComponent(drawable);
    renderSystem.registerComponent(drawable);

    PhysicalObject *physical = new PhysicalCircle(ent, position, radius, velocity);
    ent->addPhysicalComponent(physical);
    physicalSystem.registerComponent(physical);

    addEntity(ent);
    return ent;
}

Entity *GameFramework::createSquare(Vec2f position, float sideLength, Color color, Vec2f velocity)
{
    Entity *ent = new Entity(this);

    Drawable *drawable = new DrawableSquare(ent, position, sideLength, color);
    ent->addDrawableComponent(drawable);
    renderSystem.registerComponent(drawable);

    PhysicalObject *physical = new PhysicalCircle2(ent, position, sideLength / 2, velocity);
    ent->addPhysicalComponent(physical);
    physicalSystem.registerComponent(physical);

    addEntity(ent);
    return ent;
}

Entity *GameFramework::createBound(Vec2f position, PhysicalBoundType boundType)
{
    Entity *ent = new Entity(this);

    PhysicalObject *physical = new PhysicalBound(ent, position, boundType);
    ent->addPhysicalComponent(physical);
    physicalSystem.registerComponent(physical);

    addEntity(ent);
    return ent;
}

void GameFramework::deleteEntities()
{
    auto it = entities.begin();
    while (it != entities.end())
    {
        if ((*it)->scheduledForDeletion)
        {
            if ((*it)->physicalComponentPresent)
                physicalSystem.unregisterComponent((*it)->physicalComponent);

            if ((*it)->drawableComponentPresent)
                renderSystem.unregisterComponent((*it)->drawableComponent);

            delete *it;
            it = entities.erase(it);
        }
        else
            it++;
    }
}