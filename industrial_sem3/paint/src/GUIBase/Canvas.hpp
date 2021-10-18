#ifndef CANVAS_HPP
#define CANVAS_HPP

#include "Widget.hpp"

class Canvas : public Widget
{
public:
    Canvas() = delete;
    Canvas(const IntRect &widgetRect, Widget *parent = nullptr);

    void setDrawingColor(const LGL::Color &color) { mDrawingColor = color; };
    void setPenSize(float penSize) {mPenSize = penSize; };

protected:
    virtual void redrawThis() override;

    virtual void onMouseDrag(const Vec2i &mousePosition) override;
    virtual void onMouseHoverBegin(const Vec2i &mousePosition) override {};
    virtual void onMouseClicked() override;
    virtual void onMouseReleased() override;
    virtual void onMouseHoverEnd() override {};

protected:
    LGL::RenderTexture mCanvas;
    bool mPenDown;

    LGL::Color mDrawingColor;
    float mPenSize;
};

#endif