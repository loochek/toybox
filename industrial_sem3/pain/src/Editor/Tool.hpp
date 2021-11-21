#ifndef TOOL_HPP
#define TOOL_HPP

#include "../Utils/Vec2.hpp"
#include "Canvas.hpp"

/**
 * Abstract tool
 */
class Tool
{
public:
    virtual const char *getName() { return "Generic tool"; };

    virtual void onMouseClicked(Canvas &canvas, const Vec2i &position) {};
    virtual void onMouseMove(Canvas &canvas, const Vec2i &position) {};
    virtual void onMouseReleased(Canvas &canvas, const Vec2i &position) {};

    void onSizeChange(int newSize) { mCurrSize = newSize; };
    void onColorChange(const LGL::Color &newColor) { mCurrColor = newColor; };

protected:
    Tool() : mCurrSize(0) {};

protected:
    int mCurrSize;
    LGL::Color mCurrColor;
};

#endif