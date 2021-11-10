#include "PaintWindow.hpp"
#include "../GUIBase/Canvas.hpp"
#include "../GUIBase/BaseButton.hpp"
#include "../GUILogic/PaintWindow/PaintWindowCloseDelegate.hpp"

PaintWindow::PaintWindow(const IntRect &canvasRect, PaintController *controller, Widget *parent) :
    Window(canvasRect, parent), mController(controller)
{
    setTitle("Pain");

    mCanvas = new Canvas(IntRect(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT), canvasRect.size),
                         this);
    addChild(mCanvas);

    mCloseButtonDelegate = new PaintWindowCloseDelegate(controller, this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

PaintWindow::~PaintWindow()
{
    delete mCloseButtonDelegate;
}

EventResult PaintWindow::onKeyPressed(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    if (key == LGL::KeyboardKey::S && LGL::isControlPressed(modifier))
    {
        mController->onCanvasSave(this);
        return EventResult::Handled;
    }

    return EventResult::Ignored;
}