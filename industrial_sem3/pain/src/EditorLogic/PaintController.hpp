#ifndef PAINT_CONTROLLER_HPP
#define PAINT_CONTROLLER_HPP

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "ColorChangeDelegate.hpp"
#include "SizePicker/SizeChangedDelegate.hpp"
#include "ToolPicker/ToolChangedDelegate.hpp"

class WindowManager;
class PaintWindow;
class PalleteWindow;
class SizePickerWindow;
class ToolPickerWindow;
class Plugin;

/**
 * App logic controller
 */
class PaintController : public ColorChangeDelegate, public SizeChangedDelegate, public ToolChangedDelegate
{
public:
    PaintController() = delete;
    PaintController(WindowManager *root);

    ~PaintController();

    PaintWindow *createCanvas();
    bool openFile(const char *fileName);
    void openPallete();
    void openSizePicker();
    void openToolPicker();
    void openSplineWindow();
    void openImageOpenWindow();

    void onCanvasClose(PaintWindow *paintWindow);
    void onCanvasSave(PaintWindow *paintWindow);

    void onPalleteClose();
    void onSizePickerClose();
    void onToolPickerClose();

    // Size picker callback
    virtual void onSizeChange(float newPenSize, int userData) override;

    // Tool picker callback
    virtual void onToolChange(Plugin *newTool, int userData) override;

    // Pallete callback
    virtual void onColorChange(const LGL::Color &color, int userData) override;

private:
    void updateTitle(PaintWindow *window, const char *newTitle);

private:
    WindowManager     *mRoot;
    PalleteWindow     *mPallete;
    SizePickerWindow  *mSizePicker;
    ToolPickerWindow  *mToolPicker;

    std::unordered_set<PaintWindow*> mPaintWindows;

    std::unordered_map<PaintWindow*, char*> mWindowsNames;

    Plugin *mCurrTool;

    LGL::Color mCurrColor;
    float      mCurrToolSize;
};

#endif