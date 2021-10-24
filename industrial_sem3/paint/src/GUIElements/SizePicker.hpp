#ifndef SIZE_PICKER_HPP
#define SIZE_PICKER_HPP

#include "../GUIBase/Widget.hpp"

class Canvas;
class SizePickerDelegate;

/**
 * Canvas size picker widget
 */
class SizePicker : public Widget
{
public:
    SizePicker() = delete;
    SizePicker(const Vec2i &position, Widget *parent = nullptr);

    virtual ~SizePicker();

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
    static const int *PICKER_SIZES;

protected:
    SizePickerDelegate *mDelegate;
};

#endif