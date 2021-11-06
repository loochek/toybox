#ifndef SIZE_PICKER_HPP
#define SIZE_PICKER_HPP

#include <unordered_set>
#include "../GUIBase/Widget.hpp"

class Canvas;
class SizeChangedDelegate;
class SizePickerSliderDelegate;

class SizePicker : public Widget
{
public:
    SizePicker() = delete;
    SizePicker(const Vec2i &position, Widget *parent = nullptr);

    virtual ~SizePicker();

    /**
     * Sets delegate for the size picker
     * It is triggered when selected size changed
     * 
     * \param delegate Delegate or null pointer
     */
    void setDelegate(SizeChangedDelegate *delegate) { mDelegate = delegate; }

public:
    static const Vec2i PICKER_SIZE;

protected:
    SizeChangedDelegate      *mDelegate;
    SizePickerSliderDelegate *mSliderDelegate;

    friend class SizePickerSliderDelegate;
};

#endif