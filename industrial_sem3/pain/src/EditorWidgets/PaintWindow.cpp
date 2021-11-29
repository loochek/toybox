#include "PaintWindow.hpp"
#include "../Editor/PluginManager.hpp"
#include "../BaseGUI/BaseButton.hpp"
#include "CanvasWidget.hpp"
#include "../EditorLogic/PaintWindow/PaintWindowCloseDelegate.hpp"

PaintWindow::PaintWindow(const IntRect &canvasRect, PaintController *controller, Widget *parent) :
    Window(canvasRect, parent), mController(controller)
{
    setTitle("Pain");

    mCanvasWidget = new CanvasWidget(IntRect(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT),
                                             canvasRect.size),
                                     this);
    addChild(mCanvasWidget);

    mCloseButtonDelegate = new PaintWindowCloseDelegate(controller, this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

PaintWindow::~PaintWindow()
{
    delete mCloseButtonDelegate;
}

void PaintWindow::onMouseClickedThis(const Vec2i &localMousePos, const Vec2i &globalMousePos)
{
    PluginManager::getInstance()->setActiveCanvas(&mCanvasWidget->getCanvas());
}

EventResult PaintWindow::onKeyPressed(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    if (LGL::isControlPressed(modifier))
    {
        if (key == LGL::KeyboardKey::S)
        {
            mController->onCanvasSave(this);
            return EventResult::Handled;
        }
        else if (key == LGL::KeyboardKey::Z)
        {
            mCanvasWidget->getCanvas().undo();
            return EventResult::Handled;
        }
        else if (key == LGL::KeyboardKey::L)
        {
            mCanvasWidget->getCanvas().loadFromFile("test.jpg");
            return EventResult::Handled;
        }
    }

    return EventResult::Ignored;
}