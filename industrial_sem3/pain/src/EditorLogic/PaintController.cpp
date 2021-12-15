#include <cstring>
#include <cassert>
#include <filesystem>
#include "PaintController.hpp"
#include "../Utils/Logger.hpp"
#include "../Editor/PluginManager.hpp"
#include "../BaseGUI/Label.hpp"
#include "../BaseGUI/ButtonBar.hpp"
#include "../EditorWidgets/PaintMainWindow.hpp"
#include "../EditorWidgets/CanvasWidget.hpp"
#include "../EditorWidgets/PaintWindow.hpp"
#include "../EditorWidgets/PalleteWindow.hpp"
#include "../EditorWidgets/Pallete.hpp"
#include "../EditorWidgets/SizePickerWindow.hpp"
#include "../EditorWidgets/SizePicker.hpp"
#include "../EditorWidgets/PluginPickerWindow.hpp"
#include "../EditorWidgets/PluginPicker.hpp"
#include "../EditorWidgets/SplineWindow.hpp"
#include "../EditorWidgets/ImageOpenWindow.hpp"
//#include "../EditorWidgets/PluginConfigWindow.hpp"

const int MAX_FILE_NAME_SIZE = 256;

const IntRect CANVAS_INIT_RECT        = IntRect(Vec2i(200, 200), Vec2i(700, 400));
const IntRect SPLINE_WINDOW_INIT_RECT = IntRect(Vec2i(100, 100), Vec2i(500, 500));
const Vec2i   IMAGE_OPEN_INIT_POS     = Vec2i(200, 200);
const Vec2i   COLOR_PICKER_INIT_POS   = Vec2i(1000, 300);
const Vec2i   SIZE_PICKER_INIT_POS    = Vec2i(1000, 100);
const Vec2i   TOOL_PICKER_INIT_POS    = Vec2i(500, 300);
const Vec2i   EFFECT_PICKER_INIT_POS  = Vec2i(700, 300);
const Vec2i   PLUGIN_CONFIG_INIT_POS  = Vec2i(200, 200);

PaintController::PaintController(PaintMainWindow *root) : 
    mRoot(root), mPallete(nullptr), mSizePicker(nullptr), mToolPicker(nullptr), mEffectPicker(nullptr),
    mImageOpenWindow(nullptr), mCurrToolSize(2.0f), mCurrTool(nullptr), mActivePaintWindow(nullptr)
{
    root->mMenuBar->addButton("New canvas"   , this, (int)MenuAction::NewCanvas);
    root->mMenuBar->addButton("Open image"   , this, (int)MenuAction::OpenImageOpenWindow);
    root->mMenuBar->addButton("Pallete"      , this, (int)MenuAction::OpenPallete);
    root->mMenuBar->addButton("Size picker"  , this, (int)MenuAction::OpenSizePicker);
    root->mMenuBar->addButton("Tool picker"  , this, (int)MenuAction::OpenToolPicker);
    root->mMenuBar->addButton("Effect picker", this, (int)MenuAction::OpenEffectPicker);
    root->mMenuBar->addButton("Spline"       , this, (int)MenuAction::OpenSplineWindow);

    loadPlugins();
}

PaintController::~PaintController()
{
    PluginManager::getInstance()->deinit();
    
    for (auto iter = mWindowsFileNames.begin(); iter != mWindowsFileNames.end(); iter++)
        delete[] iter->second;
}

void PaintController::createCanvas()
{
    PaintWindow *paintWindow = createCanvasGeneric();

    mWindowsFileNames[paintWindow] = new char[MAX_FILE_NAME_SIZE + 1]();
    snprintf(mWindowsFileNames[paintWindow], MAX_FILE_NAME_SIZE, "Untitled %u.png", rand());
    updateTitle(paintWindow, mWindowsFileNames[paintWindow]);

    const BaseButton *taskBarButton = mRoot->mTaskBar->addButton(mWindowsFileNames[paintWindow], this,
                                                                 (int64_t)paintWindow);
    mTaskBarButtons[paintWindow] = taskBarButton;
}

bool PaintController::openFile(const char *fileName)
{
    PaintWindow *paintWindow = createCanvasGeneric();

    /// TODO: Fix unsucessful opening

    if (!paintWindow->getCanvasWidget()->getCanvas().loadFromFile(fileName))
        return false;

    Vec2i imageSize = paintWindow->getCanvasWidget()->getCanvas().getSize();
    paintWindow->getCanvasWidget()->resize(imageSize);
    paintWindow->resize(imageSize);

    mWindowsFileNames[paintWindow] = new char[MAX_FILE_NAME_SIZE + 1]();
    strncpy(mWindowsFileNames[paintWindow], fileName, MAX_FILE_NAME_SIZE);
    updateTitle(paintWindow, mWindowsFileNames[paintWindow]);

    const BaseButton *taskBarButton = mRoot->mTaskBar->addButton(mWindowsFileNames[paintWindow], this,
                                                                 (int64_t)paintWindow);
    mTaskBarButtons[paintWindow] = taskBarButton;

    return true;
}

void PaintController::openPallete()
{
    if (mPallete != nullptr)
    {
        mRoot->popUp(mPallete);
        return;
    }

    mPallete = new PalleteWindow(COLOR_PICKER_INIT_POS, this, mRoot);
    mPallete->getPallete()->setDelegate(this);

    mRoot->addChild(mPallete);
}

void PaintController::openSizePicker()
{
    if (mSizePicker != nullptr)
    {
        mRoot->popUp(mSizePicker);
        return;
    }

    mSizePicker = new SizePickerWindow(SIZE_PICKER_INIT_POS, this, mRoot);
    mSizePicker->getSizePicker()->setDelegate(this);

    mRoot->addChild(mSizePicker);
}

void PaintController::openToolPicker()
{
    if (mToolPicker != nullptr)
    {
        mRoot->popUp(mToolPicker);
        return;
    }

    mToolPicker = new PluginPickerWindow(TOOL_PICKER_INIT_POS, this, PUPPY::PluginType::TOOL, mRoot);
    mToolPicker->getPluginPicker()->setDelegate(this);

    mRoot->addChild(mToolPicker);
}

void PaintController::openEffectPicker()
{
    if (mEffectPicker != nullptr)
    {
        mRoot->popUp(mEffectPicker);
        return;
    }

    mEffectPicker = new PluginPickerWindow(EFFECT_PICKER_INIT_POS, this, PUPPY::PluginType::EFFECT, mRoot);
    mEffectPicker->getPluginPicker()->setDelegate(this);

    mRoot->addChild(mEffectPicker);
}

void PaintController::openSplineWindow()
{
    mRoot->addChild(new SplineWindow(SPLINE_WINDOW_INIT_RECT, mRoot));
}

void PaintController::openImageOpenWindow()
{
    if (mImageOpenWindow != nullptr)
    {
        mRoot->popUp(mImageOpenWindow);
        return;
    }

    mImageOpenWindow = new ImageOpenWindow(IMAGE_OPEN_INIT_POS, this, mRoot);
    mRoot->addChild(mImageOpenWindow);
}

// PluginConfigWindow *PaintController::createPluginSettingsWindow(Plugin *plugin)
// {
//     PluginConfigWindow *configWindow = new PluginConfigWindow(PLUGIN_CONFIG_INIT_POS, this, plugin, mRoot);
//     configWindow->scheduleForDisable();
//     mRoot->addChild(configWindow);

//     const BaseButton *menuButton = mRoot->mMenuBar->addButton(plugin->getInfo()->name, this,
//                                                               (uint64_t)configWindow);

//     return configWindow;
// }

void PaintController::onCanvasClose(PaintWindow *paintWindow)
{
    if (mActivePaintWindow == paintWindow)
        mActivePaintWindow = nullptr;
        
    mPaintWindows.erase(paintWindow);
    mRoot->mTaskBar->deleteButton(mTaskBarButtons[paintWindow]);
    mTaskBarButtons.erase(paintWindow);
}

void PaintController::onCanvasSave(PaintWindow *paintWindow)
{
    paintWindow->getCanvasWidget()->getCanvas().saveToFile(mWindowsFileNames[paintWindow]);
}

void PaintController::onCanvasLayerAdd(PaintWindow *paintWindow)
{
    Canvas *canvas = &paintWindow->getCanvasWidget()->getCanvas();

    int activeLayer = canvas->getActiveLayer();
    canvas->newLayer(activeLayer);
    canvas->setActiveLayer(activeLayer + 1);

    updateTitle(paintWindow, mWindowsFileNames[paintWindow]);
}

void PaintController::onCanvasLayerRemove(PaintWindow *paintWindow)
{
    Canvas *canvas = &paintWindow->getCanvasWidget()->getCanvas();
    int activeLayer = canvas->getActiveLayer();

    canvas->deleteLayer(activeLayer);

    updateTitle(paintWindow, mWindowsFileNames[paintWindow]);
}

void PaintController::onCanvasLayerChanged(PaintWindow *paintWindow, bool up)
{
    Canvas *canvas = &paintWindow->getCanvasWidget()->getCanvas();
    int activeLayer = canvas->getActiveLayer();

    if (up)
    {
        if (activeLayer > 0)
            canvas->setActiveLayer(activeLayer - 1);
    }
    else
    {
        if (activeLayer < canvas->getLayersCount() - 1)
            canvas->setActiveLayer(activeLayer + 1);   
    }

    updateTitle(paintWindow, mWindowsFileNames[paintWindow]);
}

void PaintController::onPalleteClose()
{
    mPallete = nullptr;
}

void PaintController::onSizePickerClose()
{
    mSizePicker = nullptr;
}

void PaintController::onPluginPickerClose(PluginPickerWindow *pickerWindow)
{
    if (pickerWindow == mToolPicker)
        mToolPicker = nullptr;
    else if (pickerWindow == mEffectPicker)
        mEffectPicker = nullptr;
    else
        assert(false);
}

void PaintController::onImageOpenWindowClose()
{
    mImageOpenWindow = nullptr;
}

Canvas *PaintController::getActiveCanvas()
{
    return &mActivePaintWindow->getCanvasWidget()->getCanvas();
}

Widget *PaintController::getRoot()
{
    return mRoot;
}

void PaintController::onSizeChange(float newPenSize, uint64_t userData)
{
    mCurrToolSize = newPenSize;
}

void PaintController::onPluginChange(Plugin *selectedPlugin, uint64_t userData)
{
    switch (selectedPlugin->get_info()->type)
    {
    case PUPPY::PluginType::TOOL:
        mCurrTool = selectedPlugin;

        for (PaintWindow *paintWindow : mPaintWindows)
            paintWindow->getCanvasWidget()->getCanvas().setTool(selectedPlugin);
        
        break;

    case PUPPY::PluginType::EFFECT:
        if (mActivePaintWindow != nullptr)
            mActivePaintWindow->getCanvasWidget()->getCanvas().applyEffect(selectedPlugin);

        break;

    default:
        assert(false);
    }
}

void PaintController::onColorChange(const LGL::Color &color, uint64_t userData)
{
    mCurrColor = color;
}

void PaintController::onClick(uint64_t userData)
{
    switch ((MenuAction)userData)
    {
    case MenuAction::NewCanvas:
        createCanvas();
        return;

    case MenuAction::OpenPallete:
        openPallete();
        return;

    case MenuAction::OpenSizePicker:
        openSizePicker();
        return;

    case MenuAction::OpenToolPicker:
        openToolPicker();
        return;

    case MenuAction::OpenEffectPicker:
        openEffectPicker();
        return;

    case MenuAction::OpenSplineWindow:
        openSplineWindow();
        return;

    case MenuAction::OpenImageOpenWindow:
        openImageOpenWindow();
        return;
    }

    Widget *widget = (Widget*)userData;

    // PluginConfigWindow *configWindow = dynamic_cast<PluginConfigWindow*>(widget);
    // if (configWindow != nullptr)
    // {
    //     if (configWindow->isEnabled())
    //         mRoot->popUp(configWindow);
    //     else
    //         configWindow->scheduleForEnable();

    //     return;
    // }

    PaintWindow *paintWindow = dynamic_cast<PaintWindow*>(widget);
    if (paintWindow != nullptr)
    {
        if (paintWindow->isEnabled())
            paintWindow->scheduleForDisable();
        else
        {
            paintWindow->scheduleForEnable();
            mRoot->popUp(paintWindow);
        }

        return;
    }
}

void PaintController::updateTitle(PaintWindow *window, const char *newTitle)
{
    Canvas *canvas = &window->getCanvasWidget()->getCanvas();

    char title[MAX_LABEL_SIZE + 1] = {0};
    snprintf(title, MAX_LABEL_SIZE, "%s [layer %d/%d] - Pain",
             mWindowsFileNames[window], canvas->getActiveLayer() + 1, canvas->getLayersCount());
    window->setTitle(title);
}

void PaintController::loadPlugins()
{
    PluginManager *pluginMgr = PluginManager::getInstance();
    pluginMgr->init(this);

    for (const auto &entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
    {
        if (entry.path().extension() == ".so")
        {
            try
            {
                pluginMgr->loadPlugin(entry.path().c_str());
            }
            catch (const std::exception& error)
            {
                Logger::log(LogLevel::Warning, "%s\n", error.what());
            }
        }
    }
}

PaintWindow *PaintController::createCanvasGeneric()
{
    PaintWindow *paintWindow = new PaintWindow(CANVAS_INIT_RECT, this, mRoot);

    if (mCurrTool)
        paintWindow->getCanvasWidget()->getCanvas().setTool(mCurrTool);

    mPaintWindows.insert(paintWindow);
    mRoot->addChild(paintWindow);
    return paintWindow;
}