#include "PalleteWindow.hpp"
#include "Pallete.hpp"
#include "../GUIBase/BaseButton.hpp"
#include "../GUILogic/PalleteWindow/PalleteCloseDelegate.hpp"

PalleteWindow::PalleteWindow(const Vec2f &pickerPos, PaintController *controller, Widget *parent) :
    Window(IntRect(pickerPos, Pallete::PALLETE_SIZE), parent)
{    
    setTitle("Pallete");

    mPallete = new Pallete(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT), this);
    addChild(mPallete);

    mCloseButtonDelegate = new PalleteCloseDelegate(controller, this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

PalleteWindow::~PalleteWindow()
{
    delete mCloseButtonDelegate;
}