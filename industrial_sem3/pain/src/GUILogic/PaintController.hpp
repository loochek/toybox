#ifndef PAINT_CONTROLLER_HPP
#define PAINT_CONTROLLER_HPP

#include <unordered_set>
#include "ColorChangeDelegate.hpp"
#include "SizePicker/SizeChangedDelegate.hpp"

class WindowManager;
class PaintWindow;
class PalleteWindow;
class SizePickerWindow;

/**
 * App logic controller
 */
class PaintController : public ColorChangeDelegate, public SizeChangedDelegate
{
public:
    PaintController() = delete;
    PaintController(WindowManager *root);

    void createCanvas();
    void openPallete();
    void openSizePicker();

    void onCanvasClose(PaintWindow *paintWindow);
    void onPalleteClose();
    void onSizePickerClose();

    // Size picker callback
    virtual void onSizeChange(float newPenSize, int userData) override;

    // Pallete callback
    virtual void onColorChange(const LGL::Color &color, int userData) override;

private:
    WindowManager     *mRoot;
    PalleteWindow     *mPallete;
    SizePickerWindow  *mSizePicker;

    std::unordered_set<PaintWindow*> mPaintWindows;

    LGL::Color mCurrColor;
    float      mCurrPenSize;
};

#endif