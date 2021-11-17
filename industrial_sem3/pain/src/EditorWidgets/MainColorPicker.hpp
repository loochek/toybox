#ifndef MAIN_COLOR_PICKER_HPP
#define MAIN_COLOR_PICKER_HPP

#include "../BaseGUI/Widget.hpp"
#include "../LGL/LGL.hpp"

class ColorChangeDelegate;

class MainColorPicker : public Widget
{
public:
    MainColorPicker() = delete;
    MainColorPicker(const IntRect &widgetRect, Widget *parent = nullptr);

    /**
     * Sets key color for the picker
     * 
     * \param color Key color
     */
    void setKeyColor(const LGL::Color &color);

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
    LGL::Color calcColor(Vec2i offset);

protected:
    ColorChangeDelegate *mDelegate;

    LGL::Color mKeyColor;

    Vec2i mSelectorOffs;
    Vec2i mMouseOffs;

    LGL::Texture mGradientTexture;
    LGL::Image   mGradientImage;
    
    const LGL::Texture *mSelectorTexture;
};

#endif