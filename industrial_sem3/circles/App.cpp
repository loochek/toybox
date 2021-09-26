#include <stdexcept>
#include "App.hpp"
#include "DrawableCircle.hpp"
#include "DrawableSquare.hpp"
#include "PhysicalCircle.hpp"
#include "PhysicalCircle2.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

App::App() : graphics(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), renderSystem(graphics)
{

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            entities.insert(createCircle(Vec2f(100.0f + 30.0f * i, 100.f + 30.0f * j),
                                               10.0f, Color(1.0f, 1.0f, 0.0f), Vec2f(0.0f, 0.0f)));
        }
    }

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            entities.insert(createSquare(Vec2f(500.0f + 30.0f * i, 500.f + 30.0f * j),
                                         20.0f, Color(1.0f, 0.0f, 1.0f), Vec2f(0.0f, 0.0f)));
        }
    }

    entities.insert(createCircle(Vec2f(400.0f, 100.0f), 60.0f, Color(1.0f, 1.0f, 1.0f),
                    Vec2f(700.0f, 30.0f)));

    entities.insert(createBound(Vec2f(), PhysicalBoundType::Horizontal));
    entities.insert(createBound(Vec2f(), PhysicalBoundType::Vertical));
    entities.insert(createBound(Vec2f(WINDOW_WIDTH, WINDOW_HEIGHT), PhysicalBoundType::Horizontal));
    entities.insert(createBound(Vec2f(WINDOW_WIDTH, WINDOW_HEIGHT), PhysicalBoundType::Vertical));
}

App::~App()
{
    for (Entity *entity : entities)
    {
        delete entity;
    }
}

void App::run()
{
    while (!graphics.shouldClose())
    {
        // Update

        float elapsedTime = graphics.timerReset();
        
        for (int i = 0; i < 10; i++)
            physicalSystem.update(elapsedTime / 10.0f);

        deleteEntities();

        // Draw

        graphics.clear();
        renderSystem.draw();
        graphics.display();
    }
}

Entity *App::createCircle(Vec2f position, float radius, Color color, Vec2f velocity)
{
    Entity *ent = new Entity();

    Drawable *drawable = new DrawableCircle(ent, position, radius, color);
    ent->addDrawableComponent(drawable);
    renderSystem.registerComponent(drawable);

    PhysicalObject *physical = new PhysicalCircle(ent, position, radius, velocity);
    ent->addPhysicalComponent(physical);
    physicalSystem.registerComponent(physical);

    return ent;
}

Entity *App::createSquare(Vec2f position, float sideLength, Color color, Vec2f velocity)
{
    Entity *ent = new Entity();

    Drawable *drawable = new DrawableSquare(ent, position, sideLength, color);
    ent->addDrawableComponent(drawable);
    renderSystem.registerComponent(drawable);

    PhysicalObject *physical = new PhysicalCircle2(ent, position, sideLength / 2, velocity);
    ent->addPhysicalComponent(physical);
    physicalSystem.registerComponent(physical);

    return ent;
}

Entity *App::createBound(Vec2f position, PhysicalBoundType boundType)
{
    Entity *ent = new Entity();

    PhysicalObject *physical = new PhysicalBound(ent, position, boundType);
    ent->addPhysicalComponent(physical);
    physicalSystem.registerComponent(physical);

    return ent;
}

void App::deleteEntities()
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