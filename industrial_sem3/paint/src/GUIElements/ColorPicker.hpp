#ifndef COLOR_PICKER_HPP
#define COLOR_PICKER_HPP

#include "../GUIBase/Widget.hpp"

class Canvas;

/**
 * Canvas color picker widget
 */
class ColorPicker : public Widget
{
public:
    ColorPicker() = delete;

    /**
     * \param position Widget position
     * \param canvas Canvas to be under control
     * \param parent Parent widget
     */
    ColorPicker(const Vec2i &position, Canvas *canvas, Widget *parent = nullptr);

    static const Vec2i PICKER_SIZE;

protected:
    Canvas *mCanvas;
};

#endif