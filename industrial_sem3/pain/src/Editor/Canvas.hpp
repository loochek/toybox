#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <deque>
#include "../LGL/LGL.hpp"

class Tool;
class Effect;

class Canvas
{
public:
    Canvas(const Vec2i &canvasSize);
    ~Canvas();

    void onMouseClicked(const Vec2i &position);
    void onMouseMove(const Vec2i &position);
    void onMouseReleased(const Vec2i &position);

    void applyEffect(Effect *effect);
    void setTool(Tool *tool) { mTool = tool; };

    void undo();
    void saveToFile(const char *fileName);

    LGL::RenderTexture *getCurrentState();

private:
    void pushHistoryState();

private:
    std::deque<LGL::RenderTexture*> mHistory;
    Tool *mTool;

    Vec2i mSize;
};

#endif