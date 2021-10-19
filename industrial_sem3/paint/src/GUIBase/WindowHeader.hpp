#ifndef WINDOW_HEADER_HPP
#define WINDOW_HEADER_HPP

#include "Widget.hpp"
#include "Label.hpp"

class Window;

class WindowHeader : public Widget
{
public:
    WindowHeader() = delete;
    WindowHeader(Window *parent);

    /**
     * Sets window header title
     * 
     * \param title Title text
     */
    void setTitle(const char *title);

protected:
    virtual void onRedrawThis() override;
    virtual void onMouseMoveThis(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseClickedThis() override;
    virtual void onMouseReleasedThis() override;

public:
    static const int HEADER_HEIGHT;

protected:
    Label *mTitle;

    bool mMousePressed;
    Vec2i mOldMousePosition;
};

#endif