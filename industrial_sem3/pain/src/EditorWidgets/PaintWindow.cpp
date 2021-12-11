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
    mController->onActivePaintWindowChange(this);
}

EventResult PaintWindow::onKeyPressed(LGL::KeyboardKey key, LGL::InputModifier modifier)
{
    Canvas *canvas = &mCanvasWidget->getCanvas();

    if (LGL::isControlPressed(modifier))
    {
        switch (key)
        {
        case LGL::KeyboardKey::S:
            mController->onCanvasSave(this);
            break;

        case LGL::KeyboardKey::Z:
            canvas->undo();
            break;

        case LGL::KeyboardKey::Equal:
            mController->onCanvasLayerAdd(this);
            break;

        case LGL::KeyboardKey::Hyphen:
            mController->onCanvasLayerRemove(this);
            break;
        
        case LGL::KeyboardKey::Up:
        case LGL::KeyboardKey::Down:
            mController->onCanvasLayerChanged(this, key == LGL::KeyboardKey::Up);
            break;
        }
    }

    return EventResult::Ignored;
}