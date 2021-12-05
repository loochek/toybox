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
     * Handle event from LGL and sends translated event to the widgets
     * 
     * \param event LGL event
     */
    void handleEvent(LGL::Event &event);

    /**
     * Draws widgets
     */
    void draw();

private:
    bool deleteScheduled(Widget *widget);
    bool disableScheduled(Widget *widget);
    void enableScheduled(Widget *widget);

private:
    LGL::RenderWindow &mWindow;
    Widget *mRoot;
};

#endif