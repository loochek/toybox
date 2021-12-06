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
    IntRect getRect() const { return mRect; };

    /**
     * \return Widget's parent
     */
    Widget *getParent() const { return mParent; };

    /**
     * Sets user data for the widget. 
     * This value can be used to distinguish widgets in some cases. 
     * For example, to handle multiple widgets with single delegate
     * 
     * \param userData User data
     */
    void setUserData(uint64_t userData) { mUserData = userData; };

    /**
     * \return User data value
     */
    uint64_t getUserData() const { return mUserData; };

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
     * Resizes widget
     * 
     * \param newSize New size
     */
    virtual void resize(const Vec2i &newSize);

    /**
     * Schedules widget for deletion. 
     * When GUI manager decides to delete widget, onDestroy will be called
     */
    void scheduleForDeletion() { mScheduledForDeletion = true; };

    /**
     * Schedules widget to be enabled. 
     * When GUI manager decides to enable widget, onEnable will be called. 
     */
    void scheduleForEnable() { mScheduledForEnable = true; };

    /**
     * Schedules widget to be disabled. 
     * When GUI manager decides to disable widget, onDisable will be called. 
     * Disabled widget is not shown and doesn't receive (almost) any events
     */
    void scheduleForDisable() { mScheduledForDisable = true; };

    /**
     * \return Is widget enabled or not
     */
    bool isEnabled() { return mEnabled; };

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

    /// Called when widget should update it's state
    virtual void onUpdate(float elapsedTime);

    /// Called when widget should update it's render texture with actual state
    virtual void onRedraw();

    /// Called when widget is about to be destroyed
    virtual void onDestroy();

    /// Called when children was destroyed. Widget should get rid of invalidated pointer to it.
    virtual void onChildDestroy(Widget *child);

    /// Called when widget is about to be disabled
    virtual void onDisable();

    /// Called when children was disabled
    virtual void onChildDisable(Widget *child);

    /// Called when widget is about to be disabled
    virtual void onEnable();

    /// Called when widget was resized
    virtual void onResize() {};

    /**
     * Mouse events
     */

    /// Called when mouse enters widget rect
    virtual void onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos);

    /// Called when mouse moves inside widget rect
    virtual void onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos);

    /// Called when mouse is clicked inside widget rect
    virtual void onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos);

    /// Called when mouse is released inside widget rect
    virtual void onMouseReleased(const Vec2i &localMousePos, const Vec2i &globalMousePos);

    /// Called when mouse scrolls inside widget rect
    virtual void onMouseScroll(int scrollDelta);

    /**
     * Called when mouse goes out from widget rect IF mouse button was released.
     * The widget "holds" the mouse until the mouse button is released.
     */
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

    /// Called when keyboard key is pressed
    virtual EventResult onKeyPressed(LGL::KeyboardKey key, LGL::InputModifier modifier);

    /// Called when keyboard key is released
    virtual EventResult onKeyReleased(LGL::KeyboardKey key, LGL::InputModifier modifier);

    /// Called when keyboard focus was received
    virtual void onKeyboardFocusReceived();

    /// Called when keyboard focus was lost
    virtual void onKeyboardFocusLost();

    /**
     * Functions which called by event handlers with/instead of dispatching event to children
     */

    virtual void onRedrawThis() {};
    virtual void onUpdateThis(float elapsedTime) {};
    virtual void onDestroyThis() {};
    virtual void onDisableThis() {};
    virtual void onEnableThis() {};
    virtual void onMouseHoverBeginThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) {};
    virtual void onMouseMoveThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) {};
    virtual void onMouseClickedThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) {};
    virtual void onMouseReleasedThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) {};
    virtual void onMouseHoverEndThis() {};
    virtual void onKeyboardFocusReceivedThis() {};
    virtual void onKeyboardFocusLostThis() {};

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

    uint64_t mUserData;

    /// Widget's size and position relative to parent widget
    IntRect mRect;

    /// Widget's texture
    LGL::RenderTexture mTexture;

    bool mEnabled;
    bool mScheduledForEnable;
    bool mScheduledForDisable;
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
    friend class SplineWidget;
};

#endif