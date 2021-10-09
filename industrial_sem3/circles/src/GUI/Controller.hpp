#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "Button.hpp"
#include "CirclesWidget.hpp"

typedef Entity *(CirclesWidget::*AddObjectFunc)(const Vec2f &position, float size,
                                                const Color &color, const Vec2f &velocity);

class AddObjectDelegate : public ButtonDelegate
{
public:
    AddObjectDelegate() = delete;

    /**
     * \param circlesWidget Circles widget instance to add object to
     * \param addObjectFunc Delegate function
     */
    AddObjectDelegate(CirclesWidget *circlesWidget, AddObjectFunc addObjectFunc, float size, const Color &color) :
                      circlesWidget(circlesWidget), func(addObjectFunc), size(size), color(color) {};

    virtual void onClick(Button *button) override
    {
        (circlesWidget->*func)(circlesWidget->getRect().size / 2, size, color,
                               Vec2f(0.0f, INITIAL_SPEED).rotated(0.01 * rand()));
    }

private:
    CirclesWidget *circlesWidget;

    AddObjectFunc func;

    float size;
    Color color;
};

#endif