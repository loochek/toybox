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
#include "../EditorWidgets/TextBoxDemoWindow.hpp"

const IntRect CANVAS_INIT_RECT        = IntRect(Vec2i(200, 200), Vec2i(700, 400));
const IntRect SPLINE_WINDOW_INIT_RECT = IntRect(Vec2i(100, 100), Vec2i(500, 500));
const IntRect TEXT_BOX_DEMO_INIT_RECT = IntRect(Vec2i(200, 200), Vec2i(320, 40));
const Vec2i   COLOR_PICKER_INIT_POS   = Vec2i(1000, 300);
const Vec2i   SIZE_PICKER_INIT_POS    = Vec2i(1000, 100);
const Vec2i   TOOL_PICKER_INIT_POS    = Vec2i(500, 300);

const char *toolLibraries[] = {
    "./libBrush.so",
    "./libEraser.so",
    "./libFill.so",
    "./kctf_rainbow_stamp.so",
    "./kctf_sharpy.so"
};

PaintController::PaintController(WindowManager *root) : 
    mRoot(root), mPallete(nullptr), mSizePicker(nullptr), mToolPicker(nullptr),
    mCurrToolSize(2.0f), mCanvasesCounter(1)
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

void PaintController::createCanvas()
{
    PaintWindow *paintWindow = new PaintWindow(CANVAS_INIT_RECT, this, mRoot);

    paintWindow->setUserData(mCanvasesCounter);

    char title[MAX_LABEL_SIZE + 1] = {0};
    snprintf(title, MAX_LABEL_SIZE, "Pain - %d", mCanvasesCounter);
    paintWindow->setTitle(title);
    mCanvasesCounter++;

    mPaintWindows.insert(paintWindow);
    mRoot->addChild(paintWindow);
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

void PaintController::openTextBoxDemo()
{
    mRoot->addChild(new TextBoxDemoWindow(TEXT_BOX_DEMO_INIT_RECT, mRoot));
}

void PaintController::onCanvasClose(PaintWindow *paintWindow)
{
    mPaintWindows.erase(paintWindow);
}

void PaintController::onCanvasSave(PaintWindow *paintWindow)
{
    int canvasNum = paintWindow->getUserData();

    char fileName[MAX_LABEL_SIZE + 1] = {0};
    snprintf(fileName, MAX_LABEL_SIZE, "masterpiece%d.png", canvasNum);

    paintWindow->getCanvasWidget()->getCanvas().saveToFile(fileName);
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
    for (PaintWindow *paintWindow : mPaintWindows)
        paintWindow->getCanvasWidget()->getCanvas().setTool(newTool);
}

void PaintController::onColorChange(const LGL::Color &color, int userData)
{
    mCurrColor = color;
    PluginManager::getInstance()->onColorChange(color);
}