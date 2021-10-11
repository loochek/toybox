#ifndef GUI_MANAGER_HPP
#define GUI_MANAGER_HPP

#include <vector>
#include "GUIWidget.hpp"
#include "MouseManager.hpp"

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
     * Handles events and updates widgets
     * 
     * \param elapsedTime Time delta
     */
    void update(float elapsedTime);

    /**
     * Draws widgets
     */
    void draw();

private:
    Graphics &graphics;

    std::vector<GUIWidget*> widgets;

    // to access private fields
    friend class MouseManager;
    MouseManager mouseManager;
};

#endif