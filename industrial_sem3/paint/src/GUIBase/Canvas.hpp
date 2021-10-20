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

protected:
    virtual void onRedrawThis() override;

    virtual void onMouseMoveThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseClickedThis() override;
    virtual void onMouseReleasedThis() override;
    virtual void onMouseHoverEndThis() override;

protected:
    LGL::RenderTexture mCanvas;

    Vec2i mPenPosition;
    bool mPenDown;
    bool mDrawPen;

    LGL::Color mDrawingColor;
    float mPenSize;
};

#endif