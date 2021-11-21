#ifndef PAINT_CONTROLLER_HPP
#define PAINT_CONTROLLER_HPP

#include <unordered_set>
#include <vector>
#include "../Editor/ToolKeeper.hpp"
#include "ColorChangeDelegate.hpp"
#include "SizePicker/SizeChangedDelegate.hpp"
#include "ToolPicker/ToolChangedDelegate.hpp"

class WindowManager;
class PaintWindow;
class PalleteWindow;
class SizePickerWindow;
class ToolPickerWindow;
class Tool;

/**
 * App logic controller
 */
class PaintController : public ColorChangeDelegate, public SizeChangedDelegate, public ToolChangedDelegate
{
public:
    PaintController() = delete;
    PaintController(WindowManager *root);

    void createCanvas();
    void openPallete();
    void openSizePicker();
    void openToolPicker();
    void openSplineWindow();
    void openTextBoxDemo();

    void onCanvasClose(PaintWindow *paintWindow);
    void onCanvasSave(PaintWindow *paintWindow);
    void onCanvasUndo(PaintWindow *paintWindow);

    void onPalleteClose();
    void onSizePickerClose();
    void onToolPickerClose();

    std::vector<Tool*> &getTools() { return mToolKeeper.getTools(); };

    // Size picker callback
    virtual void onSizeChange(float newPenSize, int userData) override;

    // Tool picker callback
    virtual void onToolChange(Tool *newTool, int userData) override;

    // Pallete callback
    virtual void onColorChange(const LGL::Color &color, int userData) override;

private:
    WindowManager     *mRoot;
    PalleteWindow     *mPallete;
    SizePickerWindow  *mSizePicker;
    ToolPickerWindow  *mToolPicker;

    std::unordered_set<PaintWindow*> mPaintWindows;

    ToolKeeper mToolKeeper;

    LGL::Color mCurrColor;
    float      mCurrToolSize;

    int mCanvasesCounter;
};

#endif