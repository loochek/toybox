#include "../TextureManager.hpp"
#include "Window.hpp"
#include "Label.hpp"
#include "Button.hpp"
#include "DragArea.hpp"
#include "../GUILogic/WindowCloseDelegate.hpp"

const int Window::HEADER_HEIGHT      = 30;
const int Window::SIDE_BORDER_SIZE   = 8;
const int Window::BOTTOM_BORDER_SIZE = 8;

bool Window::sTexturesLoaded = false;
const LGL::Texture *Window::sCorner1Texture    = nullptr;
const LGL::Texture *Window::sCorner2Texture    = nullptr;
const LGL::Texture *Window::sCorner3Texture    = nullptr;
const LGL::Texture *Window::sCorner4Texture    = nullptr;
const LGL::Texture *Window::sEdgeTopTexture    = nullptr;
const LGL::Texture *Window::sEdgeLeftTexture   = nullptr;
const LGL::Texture *Window::sEdgeRightTexture  = nullptr;
const LGL::Texture *Window::sEdgeBottomTexture = nullptr;

const int CLOSE_BUTTON_WIDTH  = 20;
const int CLOSE_BUTTON_HEIGHT = 20;
const int CLOSE_BUTTON_OFFSET = 10;

const int LABEL_HEIGHT_OFFSET = 3;


Window::Window(const IntRect &contentRect, Widget *parent) :
    Widget(IntRect(contentRect.position - Vec2i(SIDE_BORDER_SIZE, HEADER_HEIGHT),
                   contentRect.size + Vec2i(SIDE_BORDER_SIZE * 2, HEADER_HEIGHT + BOTTOM_BORDER_SIZE)),
           parent),
    mTitle(nullptr)
{
    if (!sTexturesLoaded)
    {
        TextureManager *mgr = TextureManager::getInstance();

        sCorner1Texture    = mgr->getTexture("corner1");
        sCorner2Texture    = mgr->getTexture("corner2");
        sCorner3Texture    = mgr->getTexture("corner3");
        sCorner4Texture    = mgr->getTexture("corner4");
        sEdgeTopTexture    = mgr->getTexture("edge_top");
        sEdgeLeftTexture   = mgr->getTexture("edge_left");
        sEdgeRightTexture  = mgr->getTexture("edge_right");
        sEdgeBottomTexture = mgr->getTexture("edge_bottom");

        sTexturesLoaded = true;
    }
    
    DragArea *dragArea = new DragArea(IntRect(Vec2i(), Vec2i(mRect.size.x, HEADER_HEIGHT)), this, this);
    addChild(dragArea);

    Button *button = new Button(IntRect(Vec2i(mRect.size.x - CLOSE_BUTTON_OFFSET - CLOSE_BUTTON_WIDTH, 0),
                                        Vec2i(CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT)),
                                this,
                                LGL::Color::Red);

    button->setLabel("X");
    button->setDelegate(new WindowCloseDelegate(this));
    addChild(button);
}

void Window::setTitle(const char *title)
{
    if (mTitle == nullptr)
    {
        mTitle = new Label(this);
        mTitle->setPosition(Vec2i(SIDE_BORDER_SIZE, LABEL_HEIGHT_OFFSET));
        addChild(mTitle);
    }

    mTitle->setText(title);
}

void Window::onRedrawThis()
{
    // Corners

    mTexture.drawTexture(*sCorner1Texture, Vec2i());
    mTexture.drawTexture(*sCorner2Texture, Vec2i(mRect.size.x - SIDE_BORDER_SIZE, 0));
    mTexture.drawTexture(*sCorner3Texture, Vec2i(0, mRect.size.y - BOTTOM_BORDER_SIZE));
    mTexture.drawTexture(*sCorner4Texture, Vec2i(mRect.size.x - SIDE_BORDER_SIZE,
                                                 mRect.size.y - BOTTOM_BORDER_SIZE));

    int horizEdgeWidth = mRect.size.x - SIDE_BORDER_SIZE * 2;
    int vertEdgeHeight = mRect.size.y - HEADER_HEIGHT - BOTTOM_BORDER_SIZE;

    // Top edge
    for (int x = 0; x < horizEdgeWidth; x++)
        mTexture.drawTexture(*sEdgeTopTexture, Vec2i(SIDE_BORDER_SIZE + x, 0));

    // Bottom edge
    for (int x = 0; x < horizEdgeWidth; x++)
        mTexture.drawTexture(*sEdgeBottomTexture, Vec2i(SIDE_BORDER_SIZE + x, mRect.size.y - BOTTOM_BORDER_SIZE));

    // Left edge
    for (int y = 0; y < vertEdgeHeight; y++)
        mTexture.drawTexture(*sEdgeLeftTexture, Vec2i(0, HEADER_HEIGHT + y));

    // Left edge
    for (int y = 0; y < vertEdgeHeight; y++)
        mTexture.drawTexture(*sEdgeRightTexture, Vec2i(mRect.size.x - SIDE_BORDER_SIZE, HEADER_HEIGHT + y));
}