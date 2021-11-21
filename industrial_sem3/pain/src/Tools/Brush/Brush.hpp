#ifndef BRUSH_HPP
#define BRUSH_HPP

#include "../../Editor/Tool.hpp"

class Brush : public Tool
{
public:
    virtual const char *getName() override { return "Brush"; };

    virtual void onMouseClicked(Canvas &canvas, const Vec2i &position) override;
    virtual void onMouseMove(Canvas &canvas, const Vec2i &position) override;

private:
    void draw(Canvas &canvas, const Vec2i &position);
};

#endif