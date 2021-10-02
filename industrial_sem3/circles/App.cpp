#include <stdexcept>
#include "App.hpp"
#include "Render/DrawableCircle.hpp"
#include "Render/DrawableSquare.hpp"
#include "Physics/PhysicalCircle.hpp"
#include "Physics/PhysicalCircle2.hpp"

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

App::App() : graphics(Vec2i(WINDOW_WIDTH, WINDOW_HEIGHT)), renderSystem(graphics), guiManager(graphics)
{
    Button *button = new Button(Rect(Vec2f(100.0f, 100.0f), Vec2f(100.0f, 100.0f)));
    initScene();
    guiManager.addWidget(button);
    button->setDelegate(this);
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
        physicalSystem.update(elapsedTime);
 
        deleteEntities();
        for (Entity *ent : entities)
            ent->enable();

        guiManager.update(graphics.getWindowMousePosition(), graphics.isLeftMouseButtonPressed());

        // Draw

        graphics.clear();
        renderSystem.draw();
        guiManager.draw();
        graphics.display();
    }
}

void App::addEntity(Entity *entity)
{
    entities.insert(entity);
}

Entity *App::createCircle(Vec2f position, float radius, Color color, Vec2f velocity)
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

Entity *App::createSquare(Vec2f position, float sideLength, Color color, Vec2f velocity)
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

Entity *App::createBound(Vec2f position, PhysicalBoundType boundType)
{
    Entity *ent = new Entity(this);

    PhysicalObject *physical = new PhysicalBound(ent, position, boundType);
    ent->addPhysicalComponent(physical);
    physicalSystem.registerComponent(physical);

    addEntity(ent);
    return ent;
}

void App::onClick(Button *button)
{
    createCircle(Vec2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2),
                 5.0f, Color(1.0f, 1.0f, 0.0f), Vec2f(0.0f, 100.0f).rotated(0.01 * rand()));
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

void App::initScene()
{
    // const int INIT_CIRCLES_WIDTH_COUNT  = 42;
    // const int INIT_CIRCLES_HEIGHT_COUNT = 26;

    // const float INIT_OFFSET = 30.0f;
    // const float INIT_CIRCLES_STRIDE = 30.0f;

    // for (int i = 0; i < INIT_CIRCLES_WIDTH_COUNT; i++)
    // {
    //     for (int j = 0; j < INIT_CIRCLES_HEIGHT_COUNT; j++)
    //     {
    //         createCircle(Vec2f(INIT_OFFSET + INIT_CIRCLES_STRIDE * i,
    //                            INIT_OFFSET + INIT_CIRCLES_STRIDE * j),
    //                            5.0f, Color(1.0f, 1.0f, 0.0f), Vec2f(0.0f, 100.0f).rotated(0.01 * rand()));
    //     }
    // }
    
    createBound(Vec2f(), PhysicalBoundType::Horizontal);
    createBound(Vec2f(), PhysicalBoundType::Vertical);
    createBound(Vec2f(WINDOW_WIDTH, WINDOW_HEIGHT), PhysicalBoundType::Horizontal);
    createBound(Vec2f(WINDOW_WIDTH, WINDOW_HEIGHT), PhysicalBoundType::Vertical);
}