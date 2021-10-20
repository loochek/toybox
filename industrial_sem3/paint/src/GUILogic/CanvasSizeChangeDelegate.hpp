#ifndef CANVAS_SIZE_CHANGE_DELEGATE_HPP
#define CANVAS_SIZE_CHANGE_DELEGATE_HPP

#include "../Utils/Delegate.hpp"
#include "../GUIBase/Canvas.hpp"

class CanvasSizeChangeDelegate : public Delegate
{
public:
    CanvasSizeChangeDelegate() = delete;
    CanvasSizeChangeDelegate(Canvas *canvas, int size) :
        mCanvas(canvas), mSize(size) {};

    virtual void operator()() override
    {
        mCanvas->setPenSize(mSize);
    }

    void setCanvas(Canvas *canvas) { mCanvas = canvas; };
    void setSize(int size) { mSize = size; };

protected:
    Canvas *mCanvas;
    int     mSize;
};

#endif