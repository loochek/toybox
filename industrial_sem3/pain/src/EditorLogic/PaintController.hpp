#ifndef PAINT_CONTROLLER_HPP
#define PAINT_CONTROLLER_HPP

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "../Editor/PluginManager.hpp"
#include "ColorChangeDelegate.hpp"
#include "SizePicker/SizeChangedDelegate.hpp"
#include "PluginPicker/PluginChangedDelegate.hpp"
#include "../BaseGUILogic/BaseButton/ButtonDelegate.hpp"

enum class MenuAction
{
    NewCanvas           = 0,
    OpenPallete         = 1,
    OpenSizePicker      = 2,
    OpenToolPicker      = 3,
    OpenEffectPicker    = 4,
    OpenSplineWindow    = 5,
    OpenImageOpenWindow = 6
};

class PaintMainWindow;
class PaintWindow;
class PalleteWindow;
class SizePickerWindow;
class PluginPickerWindow;
class PluginConfigWindow;
class BaseButton;
class ImageOpenWindow;

/**
 * App logic controller
 */
class PaintController :
    public ColorChangeDelegate, public SizeChangedDelegate, public PluginChangedDelegate, public ButtonDelegate
{
public:
    PaintController() = delete;
    PaintController(PaintMainWindow *root);

    ~PaintController();

    void createCanvas();
    bool openFile(const char *fileName);
    void openPallete();
    void openSizePicker();
    void openToolPicker();
    void openEffectPicker();
    void openSplineWindow();
    void openImageOpenWindow();

    //PluginConfigWindow *createPluginSettingsWindow(Plugin *plugin);

    void onCanvasClose       (PaintWindow *paintWindow);
    void onCanvasSave        (PaintWindow *paintWindow);
    void onCanvasLayerAdd    (PaintWindow *paintWindow);
    void onCanvasLayerRemove (PaintWindow *paintWindow);
    void onCanvasLayerChanged(PaintWindow *paintWindow, bool up);

    void onPalleteClose();
    void onSizePickerClose();
    void onPluginPickerClose(PluginPickerWindow *pickerWindow);
    void onImageOpenWindowClose();

    void onActivePaintWindowChange(PaintWindow *activeWindow) { mActivePaintWindow = activeWindow; };

    LGL::Color getCurrColor() { return mCurrColor; };
    float      getCurrSize() { return mCurrToolSize; };
    Canvas    *getActiveCanvas();

    // Size picker callback
    virtual void onSizeChange(float newPenSize, uint64_t userData) override;

    // Plugin pickers callback
    virtual void onPluginChange(Plugin *selectedPlugin, uint64_t userData) override;

    // Pallete callback
    virtual void onColorChange(const LGL::Color &color, uint64_t userData) override;

    // Menu bar and paint windows bottom bar buttons callback
    virtual void onClick(uint64_t userData) override;

private:
    void updateTitle(PaintWindow *window, const char *newTitle);
    void loadPlugins();
    PaintWindow *createCanvasGeneric();

private:
    PaintMainWindow    *mRoot;
    PalleteWindow      *mPallete;
    SizePickerWindow   *mSizePicker;
    PluginPickerWindow *mToolPicker;
    PluginPickerWindow *mEffectPicker;
    ImageOpenWindow    *mImageOpenWindow;

    std::unordered_set<PaintWindow*> mPaintWindows;
    PaintWindow *mActivePaintWindow;

    std::unordered_map<PaintWindow*, const BaseButton*> mTaskBarButtons;

    std::unordered_map<PaintWindow*, char*> mWindowsFileNames;

    Plugin *mCurrTool;

    LGL::Color mCurrColor;
    float      mCurrToolSize;
};

#endif