#include "Brush.hpp"

void Brush::onMouseClicked(Canvas &canvas, const Vec2i &position)
{
    draw(canvas, position);
}

void Brush::onMouseMove(Canvas &canvas, const Vec2i &position)
{
    draw(canvas, position);
}

void Brush::draw(Canvas &canvas, const Vec2i &position)
{
    canvas.mHistory.back()->drawCircle(position, mCurrSize, mCurrColor);
}

extern "C" Tool *createTool()
{
    return new Brush();
}