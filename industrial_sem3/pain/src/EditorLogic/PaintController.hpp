#ifndef PAINT_CONTROLLER_HPP
#define PAINT_CONTROLLER_HPP

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "ColorChangeDelegate.hpp"
#include "SizePicker/SizeChangedDelegate.hpp"
#include "PluginPicker/PluginChangedDelegate.hpp"

class WindowManager;
class PaintWindow;
class PalleteWindow;
class SizePickerWindow;
class PluginPickerWindow;
class Plugin;

/**
 * App logic controller
 */
class PaintController : public ColorChangeDelegate, public SizeChangedDelegate, public PluginChangedDelegate
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
    void openEffectPicker();
    void openSplineWindow();
    void openImageOpenWindow();

    void onCanvasClose(PaintWindow *paintWindow);
    void onCanvasSave(PaintWindow *paintWindow);

    void onPalleteClose();
    void onSizePickerClose();
    void onPluginPickerClose(PluginPickerWindow *pickerWindow);

    void onActivePaintWindowChange(PaintWindow *activeWindow) { mActivePaintWindow = activeWindow; };

    // Size picker callback
    virtual void onSizeChange(float newPenSize, int userData) override;

    // Plugin pickers callback
    virtual void onPluginChange(Plugin *selectedPlugin, int userData) override;

    // Pallete callback
    virtual void onColorChange(const LGL::Color &color, int userData) override;

private:
    void updateTitle(PaintWindow *window, const char *newTitle);

private:
    WindowManager      *mRoot;
    PalleteWindow      *mPallete;
    SizePickerWindow   *mSizePicker;
    PluginPickerWindow *mToolPicker;
    PluginPickerWindow *mEffectPicker;

    std::unordered_set<PaintWindow*> mPaintWindows;
    PaintWindow *mActivePaintWindow;

    std::unordered_map<PaintWindow*, char*> mWindowsNames;

    Plugin *mCurrTool;

    LGL::Color mCurrColor;
    float      mCurrToolSize;
};

#endif