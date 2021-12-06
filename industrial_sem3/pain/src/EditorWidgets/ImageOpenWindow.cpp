#include "../BaseGUI/Button.hpp"
#include "../BaseGUI/TextBox.hpp"
#include "../EditorLogic/ImageOpenWindow/ImageOpenCloseDelegate.hpp"
#include "../EditorLogic/ImageOpenWindow/ImageOpenDelegate.hpp"
#include "ImageOpenWindow.hpp"

const Vec2i   CONTENT_SIZE     = Vec2i(200, 74);
const Vec2i   TEXT_BOX_OFFSET  = Vec2i(10, 10);
const IntRect OPEN_BUTTON_RECT = IntRect(Vec2i(78, 70), Vec2i(60, 30));

ImageOpenWindow::ImageOpenWindow(const Vec2i &windowPos, PaintController *controller, Widget *parent) :
    Window(IntRect(windowPos, CONTENT_SIZE), parent)
{
    setTitle("Open image");

    mFileNameBox = new TextBox(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT) + TEXT_BOX_OFFSET,
                               CONTENT_SIZE.x - TEXT_BOX_OFFSET.x * 2, this);
    addChild(mFileNameBox);

    mOpenDelegate = new ImageOpenDelegate(this, controller);

    Button *button = new Button(OPEN_BUTTON_RECT, this);
    button->setLabel("Open");
    button->setDelegate(mOpenDelegate);
    addChild(button);
    
    mCloseButtonDelegate = new ImageOpenCloseDelegate(controller, this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

ImageOpenWindow::~ImageOpenWindow()
{
    delete mCloseButtonDelegate;
    delete mOpenDelegate;
}