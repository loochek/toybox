#include "GUIManager.hpp"
#include "Widget.hpp"

GUIManager::GUIManager(LGL::RenderWindow &window, Widget *root) : mWindow(window), mRoot(root)
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
    // delete widgets marked as "scheduled for deletion"
    deleteScheduled(mRoot);
}

void GUIManager::handleEvent(LGL::Event &event)
{
    Vec2i mouseMovePos(event.mouseMove.x, event.mouseMove.y);
    Vec2i mouseButtonPos(event.mouseButton.x, event.mouseButton.y);
    
    switch (event.type)
    {
    case LGL::Event::MouseMoved:
        
        mRoot->onMouseMove(mouseMovePos, mouseMovePos);
        break;

    case LGL::Event::MouseButtonPressed:
        if (event.mouseButton.button == LGL::MouseButton::Left)
            mRoot->onMouseClicked(mouseButtonPos, mouseButtonPos);
        break;

    case LGL::Event::MouseButtonReleased:
        if (event.mouseButton.button == LGL::MouseButton::Left)
            mRoot->onMouseReleased(mouseButtonPos, mouseButtonPos);
        break;

    case LGL::Event::KeyPressed:
        mRoot->onKeyPressed(event.key.code, event.key.modifier);
        break;

    case LGL::Event::KeyReleased:
        mRoot->onKeyReleased(event.key.code, event.key.modifier);
        break;

    default:
        break;
    }
}

void GUIManager::draw()
{
    mRoot->onRedraw();
    mWindow.drawRenderTexture(mRoot->mTexture, Vec2i());
}

bool GUIManager::deleteScheduled(Widget *widget)
{
    if (widget->mScheduledForDeletion)
    {
        widget->onDestroy();
        delete widget;

        return true;
    }

    for (auto iter = widget->mChildren.begin(); iter != widget->mChildren.end(); )
    {
        if (deleteScheduled(*iter))
        {
            widget->onChildDestroy(*iter);
            widget->mChildren.erase(iter++);
        }
        else
            iter++;
    }

    return false;
}