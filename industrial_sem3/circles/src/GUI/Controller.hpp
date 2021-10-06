#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "Button.hpp"
#include "CirclesWidget.hpp"

class AddCircleButtonDelegate : public ButtonDelegate
{
public:
    AddCircleButtonDelegate() = delete;

    /**
     * \param circlesWidget Circles widget to add circle to
     */
    AddCircleButtonDelegate(CirclesWidget *circlesWidget) : circlesWidget(circlesWidget) {};

    virtual void onClick(Button *button) override
    {
        circlesWidget->createCircle(circlesWidget->getRect().size / 2, CIRCLE_SIZE, CIRCLE_COLOR,
                                    Vec2f(0.0f, INITIAL_SPEED).rotated(0.01 * rand()));
    }

private:
    CirclesWidget *circlesWidget;
};

class AddSquareButtonDelegate : public ButtonDelegate
{
public:
    AddSquareButtonDelegate() = delete;

    /**
     * \param circlesWidget Circles widget to add square to
     */
    AddSquareButtonDelegate(CirclesWidget *circlesWidget) : circlesWidget(circlesWidget) {};

    virtual void onClick(Button *button) override
    {
        circlesWidget->createSquare(circlesWidget->getRect().size / 2, SQUARE_SIZE, SQUARE_COLOR,
                                    Vec2f(0.0f, INITIAL_SPEED).rotated(0.01 * rand()));
    }

private:
    CirclesWidget *circlesWidget;
};

class AddTriangleButtonDelegate : public ButtonDelegate
{
public:
    AddTriangleButtonDelegate() = delete;

    /**
     * \param circlesWidget Circles widget to add square to
     */
    AddTriangleButtonDelegate(CirclesWidget *circlesWidget) : circlesWidget(circlesWidget) {};

    virtual void onClick(Button *button) override
    {
        circlesWidget->createTriangle(circlesWidget->getRect().size / 2, TRIANGLE_SIZE, SQUARE_COLOR,
                                      Vec2f(0.0f, INITIAL_SPEED).rotated(0.01 * rand()));
    }

private:
    CirclesWidget *circlesWidget;
};

#endif