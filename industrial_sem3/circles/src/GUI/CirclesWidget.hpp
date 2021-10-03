#ifndef CIRCLES_WIDGET_HPP
#define CIRCLES_WIDGET_HPP

#include "RectangularWidget.hpp"
#include "Button.hpp"
#include "../GameFramework/GameFramework.hpp"

class CirclesWidget : public RectangularWidget, public GameFramework, public ButtonDelegate
{
public:
    CirclesWidget() = delete;
    CirclesWidget(const Rect &rect, Graphics &graphics);

    void update(float elapsedTime) override;
    void draw(Graphics &graphics) override;

    void onClick(Button *button) override;

private:
    void initScene();

public:
    Button *addCircleButton;
    Button *addSquareButton;
};

#endif