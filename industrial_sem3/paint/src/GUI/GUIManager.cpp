#include "GUIManager.hpp"

GUIManager::GUIManager(Graphics &graphics) : graphics(graphics), 
                                             widgetUnderMouse(nullptr), mouseWasPressed(false)
{
}

void GUIManager::addWidget(GUIWidget *widget)
{
    widgets.push_back(widget);
}

void GUIManager::handleMouse(const Vec2f &mousePosition, bool mousePressed)
{
    if (widgetUnderMouse != nullptr)
    {
        bool hovered = widgetUnderMouse->testShape(mousePosition);

        if (hovered)
        {
            if (!mouseWasPressed && !mousePressed)
                widgetUnderMouse->onMouseHover();
            else if (!mouseWasPressed && mousePressed)
                widgetUnderMouse->onMouseClicked();
            else if (mouseWasPressed && !mousePressed)
                widgetUnderMouse->onMouseReleased();
            else if (mouseWasPressed && mousePressed)
                widgetUnderMouse->onMousePressed();

            mouseWasPressed = mousePressed;
            return;
        }
        else
        {
            if (mouseWasPressed)
                widgetUnderMouse->onMouseReleased();

            widgetUnderMouse->onMouseHoverEnd();
            widgetUnderMouse = nullptr;
        }
    }

    for (GUIWidget *widget : widgets)
    {
        bool hovered = widget->testShape(mousePosition);
        if (!hovered)
            continue;

        widget->onMouseHoverBegin();
        if (mousePressed)
            widget->onMouseClicked();

        widgetUnderMouse = widget;
        break;
    }

    mouseWasPressed = mousePressed;
}

void GUIManager::update(float elapsedTime)
{
    for (GUIWidget *widget : widgets)
        widget->update(elapsedTime);
}

void GUIManager::draw()
{
    for (GUIWidget *widget : widgets)
        widget->draw(graphics);
}

GUIManager::~GUIManager()
{
    for (GUIWidget *widget : widgets)
        delete widget;
}