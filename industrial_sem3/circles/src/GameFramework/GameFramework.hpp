#ifndef GAME_FRAMEWORK_HPP
#define GAME_FRAMEWORK_CPP

#include <unordered_set>
#include "Entity.hpp"
#include "../Graphics.hpp"
#include "../Math/Rect.hpp"
#include "../Physics/PhysicalSystem.hpp"
#include "../Physics/PhysicalBound.hpp"
#include "../Render/RenderSystem.hpp"

class GameFramework
{
public:
    GameFramework() = delete;
    GameFramework(Graphics &graphics, const Rect &viewport);
    
    ~GameFramework();

    void gameUpdate(float elapsedTime);
    void gameDraw();

    void addEntity(Entity *entity);

    Entity *createCircle(Vec2f position = Vec2f(), float radius = 10.0f,
                         Color color = Color(1.0f, 1.0f, 1.0f), Vec2f velocity = Vec2f());

    Entity *createSquare(Vec2f position = Vec2f(), float sideLength = 20.0f,
                         Color color = Color(1.0f, 1.0f, 1.0f), Vec2f velocity = Vec2f());

    Entity *createBound(Vec2f position = Vec2f(),
                        PhysicalBoundType boundType = PhysicalBoundType::Horizontal);

private:
    /// Deletes entities that were scheduled for deletion
    void deleteEntities();

private:
    PhysicalSystem physicalSystem;
    RenderSystem   renderSystem;

    std::unordered_set<Entity*> entities;
};

#endif