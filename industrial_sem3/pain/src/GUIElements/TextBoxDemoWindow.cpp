#include "../GUIBase/BaseButton.hpp"
#include "../GUILogic/Window/WindowCloseDelegate.hpp"
#include "../GUIBase/TextBox.hpp"
#include "TextBoxDemoWindow.hpp"

TextBoxDemoWindow::TextBoxDemoWindow(const IntRect &windowRect, Widget *parent) :
    Window(windowRect, parent)
{
    setTitle("Text box demo");

    TextBox *textBox = new TextBox(Vec2i(Window::SIDE_BORDER_SIZE, Window::HEADER_HEIGHT) + Vec2i(10, 10),
                                   60, this);
    addChild(textBox);
    
    mCloseButtonDelegate = new WindowCloseDelegate(this);
    mCloseButton->setDelegate(mCloseButtonDelegate);
}

TextBoxDemoWindow::~TextBoxDemoWindow()
{
    delete mCloseButtonDelegate;
}