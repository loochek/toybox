#include "GUIManager.hpp"
#include "MainWidget.hpp"

GUIManager::GUIManager(LGL::RenderWindow &window, MainWidget *root) : mWindow(window), mRoot(root)
{
}

GUIManager::~GUIManager()
{
    delete mRoot;
}

void GUIManager::update(float elapsedTime)
{
    mRoot->onUpdate(Vec2i(), elapsedTime);
}

void GUIManager::draw()
{
    mRoot->onRedraw();
    mWindow.drawRenderTexture(mRoot->mTexture, Vec2i());
}