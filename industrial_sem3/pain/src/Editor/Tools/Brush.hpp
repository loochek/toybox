#ifndef BRUSH_HPP
#define BRUSH_HPP

#include "../Tool.hpp"

class Brush : public Tool
{
public:
    virtual void onMouseClicked(Canvas &canvas, const Vec2i &position) override;
    virtual void onMouseMove(Canvas &canvas, const Vec2i &position) override;

private:
    void draw(Canvas &canvas, const Vec2i &position);
};

#endif