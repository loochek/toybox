#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <list>
#include "../Utils/Vec2.hpp"
#include "../Utils/Rect.hpp"
#include "../LGL/LGL.hpp"

enum class EventResult
{
    Handled,
    Ignored
};

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
     * Sets user data for the widget. 
     * This value can be used to distinguish widgets in some cases. 
     * For example, to handle multiple widgets with single delegate
     * 
     * \param userData User data
     */
    void setUserData(int userData) { mUserData = userData; };

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

    /**
     * Schedules widget for deletion. 
     * When GUI manager decides to delete widget, onDestroy will be called
     */
    void scheduleForDeletion() { mScheduledForDeletion = true; };

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

    /// Called when mouse scrolls inside widget rect
    virtual void onMouseScroll(int scrollDelta);

    /// Called when mouse goes out from widget rect
    virtual void onMouseHoverEnd();

    /**
     * Keyboard events
     * 
     * Called when keyboard key is pressed/released over this widget (when it's in focus).
     * 
     * If the event was ignored by the child in focus/there is no child in focus,
     * then onKeyPressedThis/onKeyReleasedThis is called. If the parent widget also ignored the event,
     * event is considered to be ignored by the whole widget.
     * Focus is assigned to the child by click
     */

    // Called when keyboard key is pressed
    virtual EventResult onKeyPressed(LGL::KeyboardKey key, LGL::InputModifier modifier);

    // Called when keyboard key is released
    virtual EventResult onKeyReleased(LGL::KeyboardKey key, LGL::InputModifier modifier);

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

    virtual EventResult onKeyPressedThis(LGL::KeyboardKey key, LGL::InputModifier modifier)
    {
        return EventResult::Ignored;
    }

    virtual EventResult onKeyReleasedThis(LGL::KeyboardKey key, LGL::InputModifier modifier)
    {
        return EventResult::Ignored;
    }

protected:
    Widget *mParent;
    std::list<Widget*> mChildren;

    int mUserData;

    /// Widget's size and position relative to parent widget
    IntRect mRect;

    /// Widget's texture
    LGL::RenderTexture mTexture;

    bool mScheduledForDeletion;
    
    // Data for handling mouse events

    // Can be child or nullptr
    Widget *mChildUnderMouse;
    bool    mMousePressed;

    // Data for handling keyboard events

    Widget *mChildInFocus;

    // To access protected fields and methods
    friend class WindowManager;
    friend class GUIManager;
};

#endif