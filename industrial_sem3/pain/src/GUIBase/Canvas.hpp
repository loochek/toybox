#ifndef CANVAS_HPP
#define CANVAS_HPP

#include "Widget.hpp"

class Canvas : public Widget
{
public:
    Canvas() = delete;
    Canvas(const IntRect &widgetRect, Widget *parent = nullptr);

    void setDrawingColor(const LGL::Color &color) { mDrawingColor = color; };
    void setPenSize(float penSize) { mPenSize = penSize; };
    
    void saveToFile(const char *fileName);

protected:
    virtual void onRedrawThis() override;

    virtual void onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseClicked() override;
    virtual void onMouseReleased() override;
    virtual void onMouseHoverEnd() override;

protected:
    LGL::RenderTexture mCanvas;

    Vec2i mPenPosition;
    bool mPenDown;
    bool mDrawPen;

    LGL::Color mDrawingColor;
    float mPenSize;
};

#endif