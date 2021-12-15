#include "PalleteWindow.hpp"
#include "Pallete.hpp"
#include "../BaseGUI/BaseButton.hpp"
#include "../EditorLogic/PalleteWindow/PalleteCloseDelegate.hpp"

const Vec2i PALLETE_SIZE = Vec2i(260, 260);

PalleteWindow::PalleteWindow(const Vec2f &pickerPos, PaintController *controller, Widget *parent) :
    Window(IntRect(pickerPos, PALLETE_SIZE), parent)
{    
    setTitle("Pallete");

    mPallete = new Pallete(IntRect(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT), PALLETE_SIZE), this);
    addChild(mPallete);

    mCloseButtonDelegate = new PalleteCloseDelegate(controller, this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

PalleteWindow::~PalleteWindow()
{
    delete mCloseButtonDelegate;
}