#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Widget.hpp"

class WindowHeader;

/**
 * Base class for windows
 */
class Window : public Widget
{
protected:
    Window() = delete;
    Window(const IntRect &windowRect, Widget *parent = nullptr) : Widget(windowRect, parent),
                                                                  mHeader(nullptr) {};

    /**
     * \return Window header widget or null pointer if window doesn't have this
     */
    WindowHeader *getHeader() { return mHeader; };

protected:
    WindowHeader *mHeader;
};

#endif