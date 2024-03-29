#ifndef GAME_FRAMEWORK_HPP
#define GAME_FRAMEWORK_CPP

#include <unordered_set>
#include "Entity.hpp"
#include "../Graphics.hpp"
#include "../Math/Rect.hpp"
#include "../Physics/PhysicalSystem.hpp"
#include "../Physics/PhysicalBound.hpp"
#include "../Render/RenderSystem.hpp"

const float CIRCLE_SIZE   = 5.0f;
const float SQUARE_SIZE   = 10.0f;
const float TRIANGLE_SIZE = 10.0f;

const Color CIRCLE_COLOR   = Color(0.0f, 1.0f, 1.0f);
const Color SQUARE_COLOR   = Color(0.0f, 1.0f, 0.0f);
const Color TRIANGLE_COLOR = Color(1.0f, 1.0f, 0.0f);

const float INITIAL_SPEED = 100.0f;

class GameFramework
{
public:
    GameFramework() = delete;
    GameFramework(Graphics &graphics, const Rect &viewport);
    
    ~GameFramework();

    void gameUpdate(float elapsedTime);
    void gameDraw();

    void addEntity(Entity *entity);

    Entity *createCircle(const Vec2f &position = Vec2f(), float radius = CIRCLE_SIZE,
                         const Color &color = CIRCLE_COLOR, const Vec2f &velocity = Vec2f());

    Entity *createSquare(const Vec2f &position = Vec2f(), float sideLength = SQUARE_SIZE,
                         const Color &color = SQUARE_COLOR, const Vec2f &velocity = Vec2f());

    Entity *createTriangle(const Vec2f &position = Vec2f(), float sideLength = TRIANGLE_SIZE,
                           const Color &color = TRIANGLE_COLOR, const Vec2f &velocity = Vec2f());

    Entity *createBound(const Vec2f &position = Vec2f(),
                        PhysicalBoundType boundType = PhysicalBoundType::Horizontal);

    int getCirclesCount() { return circlesCount; };
    int getSquaresCount() { return squaresCount; };

private:
    /// Deletes entities that were scheduled for deletion
    void deleteEntities();

private:
    PhysicalSystem physicalSystem;
    RenderSystem   renderSystem;

    std::unordered_set<Entity*> entities;

    int circlesCount;
    int squaresCount;
};

#endif