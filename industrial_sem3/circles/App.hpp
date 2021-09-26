#ifndef APP_HPP
#define APP_HPP

#include "Graphics.hpp"
#include "PhysicalSystem.hpp"
#include "RenderSystem.hpp"
#include "Entity.hpp"
#include "PhysicalBound.hpp"

/**
 * App framework
 */
class App
{
public:
    App();
    ~App();

    /// Runs app until exit
    void run();

private:
    Entity *createCircle(Vec2f position = Vec2f(), float radius = 10.0f,
                         Color color = Color(1.0f, 1.0f, 1.0f), Vec2f velocity = Vec2f());

    Entity *createSquare(Vec2f position = Vec2f(), float sideLength = 20.0f,
                         Color color = Color(1.0f, 1.0f, 1.0f), Vec2f velocity = Vec2f());

    Entity *createBound(Vec2f position = Vec2f(),
                        PhysicalBoundType boundType = PhysicalBoundType::Horizontal);

    Graphics graphics;

    PhysicalSystem physicalSystem;
    RenderSystem   renderSystem;

    Entity **entities;
    int entitiesCount;
};

#endif