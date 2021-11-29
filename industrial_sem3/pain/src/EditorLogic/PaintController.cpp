#include <cstring>
#include "PaintController.hpp"
#include "../Utils/Logger.hpp"
#include "../Editor/PluginManager.hpp"
#include "../BaseGUI/WindowManager.hpp"
#include "../BaseGUI/Label.hpp"
#include "../EditorWidgets/CanvasWidget.hpp"
#include "../EditorWidgets/PaintWindow.hpp"
#include "../EditorWidgets/PalleteWindow.hpp"
#include "../EditorWidgets/Pallete.hpp"
#include "../EditorWidgets/SizePickerWindow.hpp"
#include "../EditorWidgets/SizePicker.hpp"
#include "../EditorWidgets/ToolPickerWindow.hpp"
#include "../EditorWidgets/ToolPicker.hpp"
#include "../EditorWidgets/SplineWindow.hpp"
#include "../EditorWidgets/ImageOpenWindow.hpp"

const int MAX_FILE_NAME_SIZE = 256;

const IntRect CANVAS_INIT_RECT        = IntRect(Vec2i(200, 200), Vec2i(700, 400));
const IntRect SPLINE_WINDOW_INIT_RECT = IntRect(Vec2i(100, 100), Vec2i(500, 500));
const Vec2i   IMAGE_OPEN_INIT_POS     = Vec2i(200, 200);
const Vec2i   COLOR_PICKER_INIT_POS   = Vec2i(1000, 300);
const Vec2i   SIZE_PICKER_INIT_POS    = Vec2i(1000, 100);
const Vec2i   TOOL_PICKER_INIT_POS    = Vec2i(500, 300);

const char *toolLibraries[] = {
    "./loochek_brush.so",
    "./loochek_eraser.so",
    "./loochek_fill.so",
    "./kctf_rainbow_stamp.so",
    "./kctf_sharpy.so",
    "./kctf_shdr.so"
};

PaintController::PaintController(WindowManager *root) : 
    mRoot(root), mPallete(nullptr), mSizePicker(nullptr), mToolPicker(nullptr),
    mCurrToolSize(2.0f), mCurrTool(nullptr)
{
    PluginManager *pluginMgr = PluginManager::getInstance();

    for (int i = 0; i < sizeof(toolLibraries) / sizeof(toolLibraries[0]); i++)
    {
        try
        {
            pluginMgr->loadPlugin(toolLibraries[i]);
        }
        catch (const std::exception& error)
        {
            Logger::log(LogLevel::Warning, "%s\n", error.what());
        }
    }

    pluginMgr->onSizeChange(mCurrToolSize);
    pluginMgr->onColorChange(mCurrColor);
}

PaintController::~PaintController()
{
    for (auto iter = mWindowsNames.begin(); iter != mWindowsNames.end(); iter++)
        delete[] iter->second;
}

PaintWindow *PaintController::createCanvas()
{
    PaintWindow *paintWindow = new PaintWindow(CANVAS_INIT_RECT, this, mRoot);

    mWindowsNames[paintWindow] = new char[MAX_FILE_NAME_SIZE + 1]();
    snprintf(mWindowsNames[paintWindow], MAX_FILE_NAME_SIZE, "Untitled %u.png", rand());

    updateTitle(paintWindow, mWindowsNames[paintWindow]);

    if (mCurrTool)
        paintWindow->getCanvasWidget()->getCanvas().setTool(mCurrTool);

    mPaintWindows.insert(paintWindow);
    mRoot->addChild(paintWindow);
    return paintWindow;
}

bool PaintController::openFile(const char *fileName)
{
    PaintWindow *paintWindow = createCanvas();

    if (!paintWindow->getCanvasWidget()->getCanvas().loadFromFile(fileName))
        return false;

    strncpy(mWindowsNames[paintWindow], fileName, MAX_FILE_NAME_SIZE);
    updateTitle(paintWindow, mWindowsNames[paintWindow]);

    Vec2i imageSize = paintWindow->getCanvasWidget()->getCanvas().getSize();
    paintWindow->resize(imageSize);
    paintWindow->getCanvasWidget()->resize(imageSize);

    return true;
}

void PaintController::openPallete()
{
    if (mPallete != nullptr)
        return;

    mPallete = new PalleteWindow(COLOR_PICKER_INIT_POS, this, mRoot);
    mPallete->getPallete()->setDelegate(this);

    mRoot->addChild(mPallete);
}

void PaintController::openSizePicker()
{
    if (mSizePicker != nullptr)
        return;

    mSizePicker = new SizePickerWindow(SIZE_PICKER_INIT_POS, this, mRoot);
    mSizePicker->getSizePicker()->setDelegate(this);

    mRoot->addChild(mSizePicker);
}

void PaintController::openToolPicker()
{
    if (mToolPicker != nullptr)
        return;

    mToolPicker = new ToolPickerWindow(TOOL_PICKER_INIT_POS, this, mRoot);
    mToolPicker->getToolPicker()->setDelegate(this);

    mRoot->addChild(mToolPicker);
}

void PaintController::openSplineWindow()
{
    mRoot->addChild(new SplineWindow(SPLINE_WINDOW_INIT_RECT, mRoot));
}

void PaintController::openImageOpenWindow()
{
    mRoot->addChild(new ImageOpenWindow(IMAGE_OPEN_INIT_POS, this, mRoot));
}

void PaintController::onCanvasClose(PaintWindow *paintWindow)
{
    mPaintWindows.erase(paintWindow);
}

void PaintController::onCanvasSave(PaintWindow *paintWindow)
{
    paintWindow->getCanvasWidget()->getCanvas().saveToFile(mWindowsNames[paintWindow]);
}

void PaintController::onPalleteClose()
{
    mPallete = nullptr;
}

void PaintController::onSizePickerClose()
{
    mSizePicker = nullptr;
}

void PaintController::onToolPickerClose()
{
    mToolPicker = nullptr;
}

void PaintController::onSizeChange(float newPenSize, int userData)
{
    mCurrToolSize = newPenSize;
    PluginManager::getInstance()->onSizeChange(newPenSize);
}

void PaintController::onToolChange(Plugin *newTool, int userData)
{
    mCurrTool = newTool;

    for (PaintWindow *paintWindow : mPaintWindows)
        paintWindow->getCanvasWidget()->getCanvas().setTool(newTool);
}

void PaintController::onColorChange(const LGL::Color &color, int userData)
{
    mCurrColor = color;
    PluginManager::getInstance()->onColorChange(color);
}

void PaintController::updateTitle(PaintWindow *window, const char *newTitle)
{
    char title[MAX_LABEL_SIZE + 1] = {0};
    snprintf(title, MAX_LABEL_SIZE, "%s - Pain", mWindowsNames[window]);
    window->setTitle(title);
}