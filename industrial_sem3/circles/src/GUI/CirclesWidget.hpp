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

    /**
     * Use it to specify the button responsible for adding circles. 
     * Callback needs this information
     * 
     * \param button Button
     */
    void setCircleAddButton(Button *button) { addCircleButton = button; };

    /**
     * Use it to specify the button responsible for adding squares. 
     * Callback needs this information
     * 
     * \param button Button
     */
    void setSquareAddButton(Button *button) { addSquareButton = button; };

private:
    void initScene();

private:
    Button *addCircleButton;
    Button *addSquareButton;
};

#endif