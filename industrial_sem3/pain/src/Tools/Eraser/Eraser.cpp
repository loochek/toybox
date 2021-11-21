#include "Eraser.hpp"

void Eraser::onMouseClicked(Canvas &canvas, const Vec2i &position)
{
    erase(canvas, position);
}

void Eraser::onMouseMove(Canvas &canvas, const Vec2i &position)
{
    erase(canvas, position);
}

void Eraser::erase(Canvas &canvas, const Vec2i &position)
{    
    canvas.mHistory.back()->drawRect(FloatRect(Vec2f(position) - Vec2f(mCurrSize, mCurrSize),
                                               Vec2f(mCurrSize, mCurrSize) * 2),
                                     LGL::Color::Transparent);
}

extern "C" Tool *createTool()
{
    return new Eraser();
}