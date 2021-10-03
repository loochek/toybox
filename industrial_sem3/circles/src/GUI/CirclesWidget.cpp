#include "../Graphics.hpp"
#include "CirclesWidget.hpp"
#include "../Render/DrawableCircle.hpp"
#include "../Render/DrawableSquare.hpp"
#include "../Physics/PhysicalCircle.hpp"
#include "../Physics/PhysicalCircle2.hpp"

const Color BACKGROUND_COLOR = Color(0.2f, 0.2f, 0.2f);

CirclesWidget::CirclesWidget(const Rect &rect, Graphics &graphics) :
                             RectangularWidget(rect), GameFramework(graphics, rect)
{
    initScene();
}

void CirclesWidget::update(float elapsedTime)
{
    gameUpdate(elapsedTime);
}

void CirclesWidget::draw(Graphics &graphics)
{
    graphics.drawRect(rect, BACKGROUND_COLOR);
    gameDraw();
}

void CirclesWidget::onClick(Button *button)
{
    if (button == addCircleButton)
    {
        createCircle(rect.size / 2, CIRCLE_SIZE, CIRCLE_COLOR,
                     Vec2f(0.0f, INITIAL_SPEED).rotated(0.01 * rand()));
    }

    if (button == addSquareButton)
    {
        createSquare(rect.size / 2, SQUARE_SIZE, SQUARE_COLOR,
                     Vec2f(0.0f, INITIAL_SPEED).rotated(0.01 * rand()));
    }       
}

void CirclesWidget::initScene()
{    
    createBound(Vec2f(), PhysicalBoundType::Horizontal);
    createBound(Vec2f(), PhysicalBoundType::Vertical);
    createBound(rect.size, PhysicalBoundType::Horizontal);
    createBound(rect.size, PhysicalBoundType::Vertical);
}