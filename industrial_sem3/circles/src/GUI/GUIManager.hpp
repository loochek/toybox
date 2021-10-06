#ifndef GUI_MANAGER_HPP
#define GUI_MANAGER_HPP

#include <vector>
#include "GUIWidget.hpp"

class Graphics;

class GUIManager
{
public:
    /**
     * \param graphics Graphics context for widget drawing and mouse handling
     */
    GUIManager(Graphics &graphics);
    ~GUIManager();

    /**
     * Adds GUI widget to be controlled
     * 
     * \param widget GUI widget
     */
    void addWidget(GUIWidget *widget);

    /**
     * Updates widgets
     * 
     * \param elapsedTime Time delta
     */
    void update(float elapsedTime);

    /**
     * Updates mouse position and sends mouse events to the widgets
     * 
     * \param mousePosition Mouse position
     * \param mousePressed Mouse button state
     */
    void handleMouse(const Vec2f &mousePosition, bool mousePressed);

    /**
     * Draws widgets
     */
    void draw();

private:
    Graphics &graphics;

    std::vector<GUIWidget*> widgets;

    GUIWidget *widgetUnderMouse;
    bool mouseWasPressed;
};

#endif