#ifndef SIZE_PICKER_HPP
#define SIZE_PICKER_HPP

#include <unordered_set>
#include "../GUIBase/Widget.hpp"

class Label;
class Slider;
class TextBox;
class SizeChangedDelegate;
class SizePickerController;

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

protected:
    virtual void onRedrawThis() override;

public:
    static const Vec2i PICKER_SIZE;

protected:
    SizeChangedDelegate  *mDelegate;
    SizePickerController *mController;

    Label   *mLabel;
    Slider  *mSlider;
    TextBox *mTextBox;

    int mCurrBrushSize;

    friend class SizePickerController;
};

#endif