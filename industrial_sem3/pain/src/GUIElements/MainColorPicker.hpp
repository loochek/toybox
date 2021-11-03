#ifndef MAIN_COLOR_PICKER_HPP
#define MAIN_COLOR_PICKER_HPP

#include "../GUIBase/Widget.hpp"
#include "../LGL/LGL.hpp"

class MainColorPicker : public Widget
{
public:
    MainColorPicker() = delete;
    MainColorPicker(const IntRect &widgetRect, Widget *parent = nullptr);

    void setKeyColor(const LGL::Color &color);

    virtual void onRedrawThis() override;

    // virtual void onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    // virtual void onMouseClicked() override;
    // virtual void onMouseReleased() override;

private:
    void redrawGradient();
    LGL::Color calcColor(int positionX, int positionY);

protected:
    LGL::Texture mGradientTexture;

    LGL::Color mKeyColor;
};

#endif