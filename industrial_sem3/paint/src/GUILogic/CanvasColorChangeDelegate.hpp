#ifndef CANVAS_COLOR_CHANGE_DELEGATE_HPP
#define CANVAS_COLOR_CHANGE_DELEGATE_HPP

#include "../Utils/Delegate.hpp"
#include "../LGL/LGL.hpp"
#include "../GUIBase/Canvas.hpp"

class CanvasColorChangeDelegate : public Delegate
{
public:
    CanvasColorChangeDelegate() = delete;
    CanvasColorChangeDelegate(Canvas *canvas, const LGL::Color &color) :
        mCanvas(canvas), mColor(color) {};

    virtual void operator()() override
    {
        mCanvas->setDrawingColor(mColor);
    }

    void setCanvas(Canvas *canvas) { mCanvas = canvas; };
    void setColor(const LGL::Color &color) { mColor = color; };

protected:
    Canvas     *mCanvas;
    LGL::Color  mColor;
};

#endif