#ifndef APP_HPP
#define APP_HPP

#include <unordered_set>
#include "Graphics.hpp"
#include "GUIManager.hpp"
#include "PhysicalSystem.hpp"
#include "RenderSystem.hpp"
#include "Entity.hpp"
#include "PhysicalBound.hpp"
#include "Button.hpp"

/**
 * App framework
 */
class App : public ButtonDelegate
{
public:
    App();
    ~App();

    /// Runs app until exit
    void run();

    void addEntity(Entity *entity);

    Entity *createCircle(Vec2f position = Vec2f(), float radius = 10.0f,
                         Color color = Color(1.0f, 1.0f, 1.0f), Vec2f velocity = Vec2f());

    Entity *createSquare(Vec2f position = Vec2f(), float sideLength = 20.0f,
                         Color color = Color(1.0f, 1.0f, 1.0f), Vec2f velocity = Vec2f());

    Entity *createBound(Vec2f position = Vec2f(),
                        PhysicalBoundType boundType = PhysicalBoundType::Horizontal);

    void onClick(Button *button);

private:
    /// Deletes entities that were scheduled for deletion
    void deleteEntities();

    void initScene();

private:
    Graphics graphics;

    GUIManager guiManager;

    PhysicalSystem physicalSystem;
    RenderSystem   renderSystem;

    std::unordered_set<Entity*> entities;
};

#endif