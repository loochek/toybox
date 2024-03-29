#include "GameFramework.hpp"
#include "../Physics/PhysicalCircle.hpp"
#include "../Physics/PhysicalCircle2.hpp"
#include "../Physics/InertPhysicalCircle.hpp"
#include "../Render/DrawableCircle.hpp"
#include "../Render/DrawableSquare.hpp"
#include "../Render/DrawableTriangle.hpp"

GameFramework::GameFramework(Graphics &graphics, const Rect &viewport) : renderSystem(graphics, viewport),
                             circlesCount(0), squaresCount(0)
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

Entity *GameFramework::createCircle(const Vec2f &position, float radius,
                                    const Color &color, const Vec2f &velocity)
{
    Entity *ent = new Entity(this);

    Drawable *drawable = new DrawableCircle(ent, position, radius, color);
    ent->addDrawableComponent(drawable);
    renderSystem.registerComponent(drawable);

    PhysicalObject *physical = new PhysicalCircle(ent, position, radius, velocity);
    ent->addPhysicalComponent(physical);
    physicalSystem.registerComponent(physical);

    addEntity(ent);
    circlesCount++;
    return ent;
}

Entity *GameFramework::createSquare(const Vec2f &position, float sideLength,
                                    const Color &color, const Vec2f &velocity)
{
    Entity *ent = new Entity(this);

    Drawable *drawable = new DrawableSquare(ent, position, sideLength, color);
    ent->addDrawableComponent(drawable);
    renderSystem.registerComponent(drawable);

    PhysicalObject *physical = new PhysicalCircle2(ent, position, sideLength / 2, velocity);
    ent->addPhysicalComponent(physical);
    physicalSystem.registerComponent(physical);

    addEntity(ent);
    squaresCount++;
    return ent;
}

Entity *GameFramework::createTriangle(const Vec2f &position, float sideLength,
                                      const Color &color, const Vec2f &velocity)
{
    Entity *ent = new Entity(this);

    Drawable *drawable = new DrawableTriangle(ent, position, sideLength, color);
    ent->addDrawableComponent(drawable);
    renderSystem.registerComponent(drawable);

    PhysicalObject *physical = new InertPhysicalCircle(ent, position, sideLength / sqrt(3), velocity);
    ent->addPhysicalComponent(physical);
    physicalSystem.registerComponent(physical);

    addEntity(ent);
    return ent;
}

Entity *GameFramework::createBound(const Vec2f &position, PhysicalBoundType boundType)
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
            if ((*it)->physicalComponent != nullptr)
                physicalSystem.unregisterComponent((*it)->physicalComponent);

            if ((*it)->drawableComponent != nullptr)
                renderSystem.unregisterComponent((*it)->drawableComponent);

            if ((*it)->physicalComponent->getType() == PhysicalObjectType::Circle)
                circlesCount--;

            if ((*it)->physicalComponent->getType() == PhysicalObjectType::Circle2)
                squaresCount--;

            delete *it;
            it = entities.erase(it);
        }
        else
            it++;
    }
}