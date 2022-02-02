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
    // do all scheduled things
    disableScheduled(mRoot);
    enableScheduled(mRoot);
    deleteScheduled(mRoot);
}

void GUIManager::handleEvent(LGL::Event &event)
{   
    switch (event.type)
    {
    case LGL::Event::MouseMoved:
        
        mRoot->onMouseMove(event.mouseMove.position, event.mouseMove.position);
        break;

    case LGL::Event::MouseButtonPressed:
        if (event.mouseButton.button == LGL::MouseButton::Left)
            mRoot->onMouseClicked(event.mouseButton.position, event.mouseButton.position);
        break;

    case LGL::Event::MouseButtonReleased:
        if (event.mouseButton.button == LGL::MouseButton::Left)
            mRoot->onMouseReleased(event.mouseButton.position, event.mouseButton.position);
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

bool GUIManager::disableScheduled(Widget *widget)
{
    if (widget->mScheduledForDisable)
    {
        widget->onDisable();
        widget->mScheduledForDisable = false;
        widget->mEnabled = false;

        return true;
    }

    for (Widget *child : widget->mChildren)
    {
        if (disableScheduled(child))
            widget->onChildDisable(child);
    }

    return false;
}

void GUIManager::enableScheduled(Widget *widget)
{
    if (widget->mScheduledForEnable)
    {
        widget->onEnable();
        widget->mScheduledForEnable = false;
        widget->mEnabled = true;

        return;
    }

    for (Widget *child : widget->mChildren)
        enableScheduled(child);

    return;
}