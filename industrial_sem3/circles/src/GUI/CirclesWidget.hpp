#ifndef CIRCLES_WIDGET_HPP
#define CIRCLES_WIDGET_HPP

#include "RectangularWidget.hpp"
#include "../GameFramework/GameFramework.hpp"

class CirclesWidget : public RectangularWidget, public GameFramework
{
public:
    CirclesWidget() = delete;
    CirclesWidget(const Rect &rect, Graphics &graphics);

    void update(float elapsedTime) override;
    void draw(Graphics &graphics) override;

private:
    void initScene();
};

#endif