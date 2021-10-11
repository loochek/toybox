#include "../Graphics.hpp"
#include "GUIManager.hpp"
#include "MouseManager.hpp"

MouseManager::MouseManager(GUIManager *guiManager) : guiManager(guiManager), mouseWasPressed(false),
                                                     widgetUnderMouse(nullptr)
{
}

void MouseManager::sendMouseEvents()
{
    const Vec2f &mousePosition = guiManager->graphics.getWindowMousePosition();
    bool         mousePressed  = guiManager->graphics.isLeftMouseButtonPressed();

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

    for (GUIWidget *widget : guiManager->widgets)
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