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
    virtual void redrawThis() override;
    virtual void mouseDragThis(const Vec2i &mousePosition) override;
    virtual void mouseClickedThis() override;
    virtual void mouseReleasedThis() override;

public:
    static const int HEADER_HEIGHT;

protected:
    Label *mTitle;

private:
    bool mMousePressed;
    Vec2i mOldMousePosition;
};

#endif