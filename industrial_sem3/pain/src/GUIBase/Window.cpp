#include "../TextureManager.hpp"
#include "Window.hpp"
#include "Label.hpp"
#include "TextureButton.hpp"
#include "DragArea.hpp"
#include "../GUILogic/WindowCloseDelegate.hpp"

const int Window::HEADER_HEIGHT      = 30;
const int Window::SIDE_BORDER_SIZE   = 8;
const int Window::BOTTOM_BORDER_SIZE = 8;

const int CLOSE_BUTTON_WIDTH  = 49;
const int CLOSE_BUTTON_HEIGHT = 20;
const int CLOSE_BUTTON_OFFSET = 8;

const int LABEL_HEIGHT_OFFSET = 3;


Window::Window(const IntRect &contentRect, Widget *parent) :
    Widget(IntRect(contentRect.position - Vec2i(SIDE_BORDER_SIZE, HEADER_HEIGHT),
                   contentRect.size + Vec2i(SIDE_BORDER_SIZE * 2, HEADER_HEIGHT + BOTTOM_BORDER_SIZE)),
           parent),
    mTitle(nullptr)
{
    getTextures();

    mTitle = new Label(this);
    mTitle->setPosition(Vec2i(SIDE_BORDER_SIZE, LABEL_HEIGHT_OFFSET));
    addChild(mTitle);
    
    DragArea *dragArea = new DragArea(IntRect(Vec2i(), Vec2i(mRect.size.x, HEADER_HEIGHT)), this, this);
    addChild(dragArea);

    mCloseButton = new TextureButton(
        IntRect(Vec2i(mRect.size.x - CLOSE_BUTTON_OFFSET - CLOSE_BUTTON_WIDTH, 0),
                Vec2i(CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT)),
        mCloseButtonIdleTexture,
        mCloseButtonHoveredTexture,
        mCloseButtonHoveredTexture,
        this);
    addChild(mCloseButton);
}

void Window::setTitle(const char *title)
{
    mTitle->setText(title);
}

void Window::onRedrawThis()
{
    // Corners

    mTexture.drawTexture(*mCorner1Texture, Vec2i());
    mTexture.drawTexture(*mCorner2Texture, Vec2i(mRect.size.x - SIDE_BORDER_SIZE, 0));
    mTexture.drawTexture(*mCorner3Texture, Vec2i(0, mRect.size.y - BOTTOM_BORDER_SIZE));
    mTexture.drawTexture(*mCorner4Texture, Vec2i(mRect.size.x - SIDE_BORDER_SIZE,
                                                 mRect.size.y - BOTTOM_BORDER_SIZE));

    int horizEdgeWidth = mRect.size.x - SIDE_BORDER_SIZE * 2;
    int vertEdgeHeight = mRect.size.y - HEADER_HEIGHT - BOTTOM_BORDER_SIZE;

    // Top edge
    mTexture.drawTexture(*mEdgeTopTexture,
                         Vec2i(SIDE_BORDER_SIZE, 0),
                         IntRect(Vec2i(), Vec2i(horizEdgeWidth, HEADER_HEIGHT)));

    // Bottom edge
    mTexture.drawTexture(*mEdgeBottomTexture,
                         Vec2i(SIDE_BORDER_SIZE, mRect.size.y - BOTTOM_BORDER_SIZE),
                         IntRect(Vec2i(), Vec2i(horizEdgeWidth, BOTTOM_BORDER_SIZE)));

    // Left edge
    mTexture.drawTexture(*mEdgeLeftTexture,
                         Vec2i(0, HEADER_HEIGHT),
                         IntRect(Vec2i(), Vec2i(SIDE_BORDER_SIZE, vertEdgeHeight)));

    // Left edge
    mTexture.drawTexture(*mEdgeRightTexture,
                         Vec2i(mRect.size.x - SIDE_BORDER_SIZE, HEADER_HEIGHT),
                         IntRect(Vec2i(), Vec2i(SIDE_BORDER_SIZE, vertEdgeHeight)));
}

void Window::getTextures()
{
    TextureManager *mgr = TextureManager::getInstance();

    mCorner1Texture    = mgr->getTexture("corner1");
    mCorner2Texture    = mgr->getTexture("corner2");
    mCorner3Texture    = mgr->getTexture("corner3");
    mCorner4Texture    = mgr->getTexture("corner4");
    mEdgeTopTexture    = mgr->getTexture("edge_top");
    mEdgeLeftTexture   = mgr->getTexture("edge_left");
    mEdgeRightTexture  = mgr->getTexture("edge_right");
    mEdgeBottomTexture = mgr->getTexture("edge_bottom");

    mCloseButtonIdleTexture    = mgr->getTexture("close_button_idle");
    mCloseButtonHoveredTexture = mgr->getTexture("close_button_hovered");

    if (mCorner1Texture == nullptr || 
        mCorner2Texture == nullptr || 
        mCorner3Texture == nullptr || 
        mCorner4Texture == nullptr || 
        mEdgeTopTexture == nullptr || 
        mEdgeLeftTexture == nullptr ||
        mEdgeRightTexture == nullptr ||
        mEdgeBottomTexture == nullptr ||
        mCloseButtonIdleTexture == nullptr ||
        mCloseButtonHoveredTexture == nullptr)
    {
        throw std::runtime_error("Window textures are not loaded");
    }
}