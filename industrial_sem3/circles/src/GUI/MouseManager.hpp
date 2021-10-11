#ifndef MOUSE_MANAGER_HPP
#define MOUSE_MANAGER_HPP

#include "GUIWidget.hpp"

class GUIManager;

/**
 * Manager which responsible for sending mouse events
 */
class MouseManager
{
public:
    MouseManager() = delete;

    /**
     * \param guiManager GUI manager to work with
     */
    MouseManager(GUIManager *guiManager);

    void sendMouseEvents();

private:
    GUIManager *guiManager;

    GUIWidget *widgetUnderMouse;
    bool mouseWasPressed;
};

#endif