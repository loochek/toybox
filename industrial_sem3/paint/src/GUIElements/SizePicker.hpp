#ifndef SIZE_PICKER_HPP
#define SIZE_PICKER_HPP

#include <unordered_set>
#include "../GUIBase/Widget.hpp"

class Canvas;
class SizePickerSliderDelegate;

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

protected:
    void onSizeChanged(int newSize);

public:
    static const Vec2i PICKER_SIZE;

protected:
    std::unordered_set<Canvas*> mCanvases;

    SizePickerSliderDelegate *mSliderDelegate;

    // to access onSizeChanged
    friend class SizePickerSliderDelegate;
};

#endif