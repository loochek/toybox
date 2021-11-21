#ifndef ERASER_HPP
#define ERASER_HPP

#include "../../Editor/Tool.hpp"

class Eraser : public Tool
{
public:
    virtual const char *getName() override { return "Eraser"; };

    virtual void onMouseClicked(Canvas &canvas, const Vec2i &position) override;
    virtual void onMouseMove(Canvas &canvas, const Vec2i &position) override;

private:
    void erase(Canvas &canvas, const Vec2i &position);
};

#endif