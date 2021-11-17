#include "../BaseGUI/BaseButton.hpp"
#include "../BaseGUILogic/Window/WindowCloseDelegate.hpp"
#include "../BaseGUI/TextBox.hpp"
#include "TextBoxDemoWindow.hpp"

TextBoxDemoWindow::TextBoxDemoWindow(const IntRect &windowRect, Widget *parent) :
    Window(windowRect, parent)
{
    setTitle("Text box demo");

    TextBox *textBox = new TextBox(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT) + Vec2i(10, 10),
                                   300, this);
    addChild(textBox);
    
    mCloseButtonDelegate = new WindowCloseDelegate(this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

TextBoxDemoWindow::~TextBoxDemoWindow()
{
    delete mCloseButtonDelegate;
}