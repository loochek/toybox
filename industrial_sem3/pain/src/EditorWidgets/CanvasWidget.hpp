#ifndef CANVAS_WIDGET_HPP
#define CANVAS_WIDGET_HPP

#include "../BaseGUI/Widget.hpp"
#include "../Editor/Canvas.hpp"

class CanvasWidget : public Widget
{
public:
    CanvasWidget() = delete;
    CanvasWidget(const IntRect &widgetRect, Widget *parent = nullptr);

    Canvas &getCanvas() { return mCanvas; };

protected:
    virtual void onRedrawThis() override;

    virtual void onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseReleased(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseHoverEnd() override;

protected:
    Canvas mCanvas;

    Vec2i mToolPosition;
    bool mToolActive;
    bool mDrawToolPreview;

    const LGL::Texture *mBackgroundTexture;
};

#endif