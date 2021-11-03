#ifndef KEY_COLOR_PICKER_HPP
#define KEY_COLOR_PICKER_HPP

#include "../GUIBase/Widget.hpp"
#include "../LGL/LGL.hpp"

class KeyColorChangedDelegate;

class KeyColorPicker : public Widget
{
public:
    KeyColorPicker() = delete;
    KeyColorPicker(const IntRect &widgetRect, Widget *parent = nullptr);

    void setDelegate(KeyColorChangedDelegate *delegate) { mDelegate = delegate; }

protected:
    virtual void onRedrawThis() override;

    virtual void onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseClicked() override;
    virtual void onMouseReleased() override;

private:
    void redrawGradient();
    LGL::Color calcColor(int position);

protected:
    KeyColorChangedDelegate *mDelegate;

    LGL::Texture mGradientTexture;

    int mSelectorOffs;
    int mMouseOffs;
    bool mMousePressed;
};

#endif