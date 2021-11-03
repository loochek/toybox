#ifndef COLOR_PICKER_HPP
#define COLOR_PICKER_HPP

#include "../GUIBase/Widget.hpp"
#include "../GUILogic/ColorPickerController.hpp"

class ColorPickerDelegate;
class Canvas;

/**
 * Canvas color picker widget
 */
class ColorPicker : public Widget
{
public:
    ColorPicker() = delete;
    ColorPicker(const Vec2i &position, Widget *parent = nullptr);

    virtual ~ColorPicker();

    /**
     * Subscribes canvas for changing its color
     * 
     * \param canvas Canvas
     */
    void subscribeCanvas(Canvas *canvas);

    /**
     * Unsubscribes canvas from changing its color
     * 
     * \param canvas Canvas
     */
    void unsubscribeCanvas(Canvas *canvas);

public:
    static const Vec2i PICKER_SIZE;

protected:
    ColorPickerController *mController;
    //ColorPickerDelegate *mDelegate;
};

#endif