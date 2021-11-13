#ifndef KEY_COLOR_PICKER_HPP
#define KEY_COLOR_PICKER_HPP

#include "../GUIBase/Widget.hpp"
#include "../LGL/LGL.hpp"

class ColorChangeDelegate;

class KeyColorPicker : public Widget
{
public:
    KeyColorPicker() = delete;
    KeyColorPicker(const IntRect &widgetRect, Widget *parent = nullptr);

    /**
     * Sets delegate for the color picker
     * It is triggered when selected color changed
     * 
     * \param delegate Delegate or null pointer
     */
    void setDelegate(ColorChangeDelegate *delegate) { mDelegate = delegate; }

    /**
     * \return Current selected color
     */
    LGL::Color getColor() { return calcColor(mSelectorOffs); };

protected:
    virtual void onRedrawThis() override;

    virtual void onMouseHoverBegin(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseMove(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseClicked(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;
    virtual void onMouseReleased(const Vec2i &localMousePos, const Vec2i &globalMousePos) override;

private:
    void redrawGradient();
    LGL::Color calcColor(int position);

protected:
    ColorChangeDelegate *mDelegate;

    LGL::Image   mGradientImage;
    LGL::Texture mGradientTexture;

    int mSelectorOffs;
    int mMouseOffs;
    bool mMousePressed;
};

#endif