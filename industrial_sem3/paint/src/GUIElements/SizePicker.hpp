#ifndef SIZE_PICKER_HPP
#define SIZE_PICKER_HPP

#include "../GUIBase/Widget.hpp"

class Canvas;

/**
 * Canvas size picker widget
 */
class SizePicker : public Widget
{
public:
    SizePicker() = delete;

    /**
     * \param position Widget position
     * \param canvas Canvas to be under control
     * \param parent Parent widget
     */
    SizePicker(const Vec2i &position, Canvas *canvas, Widget *parent = nullptr);

    static const Vec2i PICKER_SIZE;

protected:
    Canvas *mCanvas;
};

#endif