#include "GUIManager.hpp"
#include "MainWidget.hpp"

GUIManager::GUIManager(LGL::RenderWindow &window, MainWidget *root) : mWindow(window), mRoot(root),
                                                                      mMouseWasPressed(false)
{
    // To initialize mouse state machine
    root->onMouseHoverBegin(Vec2i());
}

GUIManager::~GUIManager()
{
    delete mRoot;
}

void GUIManager::update(float elapsedTime)
{
    mRoot->onUpdate(Vec2i(), elapsedTime);
}

void GUIManager::handleMouse()
{
    Vec2i mousePosition = mWindow.getWindowMousePosition();
    bool  mousePressed  = mWindow.isLeftMouseButtonPressed();

    if (mOldMousePosition != mousePosition)
        mRoot->onMouseDrag(mousePosition);

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