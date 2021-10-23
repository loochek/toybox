#include "GUIManager.hpp"
#include "Widget.hpp"

GUIManager::GUIManager(LGL::RenderWindow &window, Widget *root) : mWindow(window), mRoot(root),
                                                                  mMouseWasPressed(false)
{
    // To initialize mouse state machine
    root->onMouseHoverBegin(Vec2i(), Vec2i());
}

GUIManager::~GUIManager()
{
    mRoot->onDestroy();
    delete mRoot;
}

void GUIManager::update(float elapsedTime)
{
    mRoot->onUpdate(elapsedTime);
}

void GUIManager::handleMouse()
{
    Vec2i mousePosition = mWindow.getWindowMousePosition();
    bool  mousePressed  = mWindow.isLeftMouseButtonPressed();

    if (mOldMousePosition != mousePosition)
        mRoot->onMouseMove(mousePosition, mousePosition);

    if (mousePressed && !mMouseWasPressed)
        mRoot->onMouseClicked();

    if (!mousePressed && mMouseWasPressed)
        mRoot->onMouseReleased();

    mOldMousePosition = mousePosition;
    mMouseWasPressed  = mousePressed;
}

void GUIManager::draw()
{
    mRoot->onRedraw();
    mWindow.drawRenderTexture(mRoot->mTexture, Vec2i());
}