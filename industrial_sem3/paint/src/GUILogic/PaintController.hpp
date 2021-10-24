#ifndef PAINT_CONTROLLER_HPP
#define PAINT_CONTROLLER_HPP

#include <unordered_set>

class WindowManager;
class PaintWindow;
class ColorPickerWindow;
class SizePickerWindow;

/**
 * App logic controller
 */
class PaintController
{
public:
    PaintController() = delete;
    PaintController(WindowManager *root);

    void createCanvas();
    void openColorPicker();
    void openSizePicker();

private:
    WindowManager     *mRoot;
    ColorPickerWindow *mColorPicker;
    SizePickerWindow  *mSizePicker;

    std::unordered_set<PaintWindow*> mCanvases;
};

#endif