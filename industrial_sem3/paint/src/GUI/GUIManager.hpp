#ifndef GUI_MANAGER_HPP
#define GUI_MANAGER_HPP

#include <vector>
#include "../LGL/LGL.hpp"

class MainWidget;

class GUIManager
{
public:
    GUIManager(LGL::RenderWindow &window, MainWidget *root);
    ~GUIManager();

    /**
     * Updates widgets
     * 
     * \param elapsedTime Time delta
     */
    void update(float elapsedTime);

    /**
     * Updates mouse position and sends mouse events to the widgets
     */
    void handleMouse();

    /**
     * Draws widgets
     */
    void draw();

private:
    LGL::RenderWindow &mWindow;

    MainWidget *mRoot;
};

#endif