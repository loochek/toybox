#include "../TextureManager.hpp"
#include "Window.hpp"
#include "Label.hpp"
#include "TextureButton.hpp"
#include "DragArea.hpp"

const int Window::HEADER_HEIGHT      = 30;
const int Window::SIDE_BORDER_SIZE   = 8;
const int Window::BOTTOM_BORDER_SIZE = 8;

const int CLOSE_BUTTON_WIDTH  = 49;
const int CLOSE_BUTTON_HEIGHT = 20;
const int CLOSE_BUTTON_OFFSET = 8;

const LGL::Color WINDOW_BACKGROUND_COLOR = LGL::Color(0.94f, 0.94f, 0.94f);


Window::Window(const IntRect &contentRect, Widget *parent) :
    Widget(IntRect(contentRect.position - Vec2i(SIDE_BORDER_SIZE, HEADER_HEIGHT),
                   contentRect.size + Vec2i(SIDE_BORDER_SIZE * 2, HEADER_HEIGHT + BOTTOM_BORDER_SIZE)),
           parent),
    mTitle(nullptr)
{
    getTextures();

    mTitle = new Label(Vec2i(SIDE_BORDER_SIZE, HEADER_HEIGHT / 2), this);
    addChild(mTitle);
    
    mDragArea = new DragArea(IntRect(Vec2i(), Vec2i(mRect.size.x, HEADER_HEIGHT)), this, this);
    addChild(mDragArea);

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

void Window::resize(const Vec2i &newContentSize)
{
    Widget::resize(newContentSize + Vec2i(2 * Window::SIDE_BORDER_SIZE,
                   Window::HEADER_HEIGHT + Window::BOTTOM_BORDER_SIZE));
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

    // Window background
    mTexture.drawRect(IntRect(Vec2i(SIDE_BORDER_SIZE, HEADER_HEIGHT),
                              Vec2i(mRect.size.x - 2 * SIDE_BORDER_SIZE,
                                    mRect.size.y - HEADER_HEIGHT - BOTTOM_BORDER_SIZE)),
                      WINDOW_BACKGROUND_COLOR);
}

void Window::onResize()
{
    mCloseButton->setPosition(Vec2i(mRect.size.x - CLOSE_BUTTON_OFFSET - CLOSE_BUTTON_WIDTH, 0));
    mDragArea->resize(Vec2i(mRect.size.x, HEADER_HEIGHT));
}

void Window::getTextures()
{
    TextureManager *mgr = TextureManager::getInstance();

    mCorner1Texture    = mgr->getTexture("window_corner1");
    mCorner2Texture    = mgr->getTexture("window_corner2");
    mCorner3Texture    = mgr->getTexture("window_corner3");
    mCorner4Texture    = mgr->getTexture("window_corner4");
    mEdgeTopTexture    = mgr->getTexture("window_edge_top");
    mEdgeLeftTexture   = mgr->getTexture("window_edge_left");
    mEdgeRightTexture  = mgr->getTexture("window_edge_right");
    mEdgeBottomTexture = mgr->getTexture("window_edge_bottom");

    mCloseButtonIdleTexture    = mgr->getTexture("window_close_button_idle");
    mCloseButtonHoveredTexture = mgr->getTexture("window_close_button_hovered");

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