#ifndef PAINT_WINDOW_HPP
#define PAINT_WINDOW_HPP

#include "../BaseGUI/Window.hpp"

class CanvasWidget;
class PaintWindowCloseDelegate;
class PaintController;

class PaintWindow : public Window
{
public:
    PaintWindow() = delete;
    PaintWindow(const IntRect &canvasRect, PaintController *controller, Widget *parent = nullptr);
    
    virtual ~PaintWindow();

    /**
     * \return Canvas widget
     */
    CanvasWidget *getCanvasWidget() { return mCanvasWidget; };

protected:
    virtual void onMouseClickedThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual EventResult onKeyPressed(LGL::KeyboardKey key, LGL::InputModifier modifier) override;

protected:
    CanvasWidget *mCanvasWidget;
    PaintWindowCloseDelegate *mCloseButtonDelegate;
    PaintController *mController;
};

#endif