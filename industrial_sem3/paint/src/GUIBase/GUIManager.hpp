#ifndef GUI_MANAGER_HPP
#define GUI_MANAGER_HPP

#include <vector>
#include "../LGL/LGL.hpp"

class Widget;

class GUIManager
{
public:
    GUIManager(LGL::RenderWindow &window, Widget *root);
    ~GUIManager();

    /**
     * Updates widgets
     * 
     * \param elapsedTime Time delta
     */
    void update(float elapsedTime);

    /**
     * Gets mouse status and sends mouse events to the widgets
     */
    void handleMouse();

    /**
     * Draws widgets
     */
    void draw();

private:
    LGL::RenderWindow &mWindow;
    Widget *mRoot;

    Vec2i mOldMousePosition;
    bool  mMouseWasPressed;
};

#endif