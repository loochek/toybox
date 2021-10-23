#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <list>
#include "../Utils/Vec2.hpp"
#include "../Utils/Rect.hpp"
#include "../LGL/LGL.hpp"

/**
 * Base class for GUI widgets
 */
class Widget
{
public:
    /**
     * Pushes children widget back to the children list. 
     * Note that widgets in the list overlap: the widget at the end of the list is in the foreground
     * 
     * \param child Child widget
     */
    void addChild(Widget *child);

    /**
     * \return Widget rect
     */
    IntRect getRect() { return mRect; };

    /**
     * \return Widget's parent
     */
    Widget *getParent() { return mParent; };

    /**
     * Sets new position for the widget relative to parent
     * 
     * \param newPosition New position
     */
    void setPosition(const Vec2i &newPosition) { mRect.position = newPosition; };

    /**
     * Moves widget
     * 
     * \param offset Position delta
     */
    void move(const Vec2i &offset) { mRect.position += offset; };

protected:
    Widget() = delete;
    
    /**
     * \param widgetRect Widget's size and position relative to parent widget
     * \param parent Widget parent
     */
    Widget(const IntRect &widgetRect, Widget *parent = nullptr);
    virtual ~Widget();

    /**
     * Event handlers
     * Call "*This" functions and/or dispatches events to children
     */

    /// Widget should update it's state
    virtual void onUpdate(float elapsedTime);

    /// Widget should update it's render texture with actual state
    virtual void onRedraw();

    /// Called when mouse enters widget rect
    virtual void onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos);

    /// Called when mouse moves inside widget rect
    virtual void onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos);

    /// Called when mouse is clicked inside widget rect
    virtual void onMouseClicked();

    /// Called when mouse is released inside widget rect
    virtual void onMouseReleased();

    /// Called when mouse goes out from widget rect
    virtual void onMouseHoverEnd();

    /// Widget must destroy yourself and it's children
    virtual void onDestroy();

    /// Called when children was destroyed
    virtual void onChildDestroy(Widget *child);

    /**
     * Functions which called by event handlers with/instead of dispatching event to children
     */

    virtual void onRedrawThis() {};
    virtual void onUpdateThis(float elapsedTime) {};
    virtual void onDestroyThis() {};

protected:
    Widget *mParent;
    std::list<Widget*> mChildren;

    /// Widget's size and position relative to parent widget
    IntRect mRect;

    /// Widget's texture
    LGL::RenderTexture mTexture;

    // To access protected methods
    friend class GUIManager;

    // Data for handling mouse events

    // Can be child or nullptr
    Widget *mChildUnderMouse;
    bool    mMousePressed;

    friend class WindowManager;
    friend class WindowCloseDelegate;
};

#endif