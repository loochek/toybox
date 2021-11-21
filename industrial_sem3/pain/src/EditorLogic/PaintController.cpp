#include "PaintController.hpp"
#include "../BaseGUI/WindowManager.hpp"
#include "../BaseGUI/Label.hpp"
#include "../EditorWidgets/CanvasWidget.hpp"
#include "../EditorWidgets/PaintWindow.hpp"
#include "../EditorWidgets/PalleteWindow.hpp"
#include "../EditorWidgets/Pallete.hpp"
#include "../EditorWidgets/SizePickerWindow.hpp"
#include "../EditorWidgets/SizePicker.hpp"
#include "../EditorWidgets/SplineWindow.hpp"
#include "../EditorWidgets/TextBoxDemoWindow.hpp"

const IntRect CANVAS_INIT_RECT        = IntRect(Vec2i(200, 200), Vec2i(700, 400));
const IntRect SPLINE_WINDOW_INIT_RECT = IntRect(Vec2i(100, 100), Vec2i(500, 500));
const IntRect TEXT_BOX_DEMO_INIT_RECT = IntRect(Vec2i(200, 200), Vec2i(320, 40));
const Vec2i   COLOR_PICKER_INIT_POS   = Vec2i(1000, 300);
const Vec2i   SIZE_PICKER_INIT_POS    = Vec2i(1000, 100);

PaintController::PaintController(WindowManager *root) : 
    mRoot(root), mPallete(nullptr), mSizePicker(nullptr), mCurrPenSize(1.0f), mCanvasesCounter(1)
{
}

void PaintController::createCanvas()
{
    PaintWindow *paintWindow = new PaintWindow(CANVAS_INIT_RECT, this, mRoot);

    paintWindow->setUserData(mCanvasesCounter);

    char title[MAX_LABEL_SIZE + 1] = {0};
    snprintf(title, MAX_LABEL_SIZE, "Pain - %d", mCanvasesCounter);
    paintWindow->setTitle(title);
    mCanvasesCounter++;

    paintWindow->getCanvasWidget()->getCanvas().setTool(&mBrush);

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

void PaintController::onCanvasUndo(PaintWindow *paintWindow)
{
    paintWindow->getCanvasWidget()->getCanvas().undo();
}

void PaintController::onPalleteClose()
{
    mPallete = nullptr;
}

void PaintController::onSizePickerClose()
{
    mSizePicker = nullptr;
}

void PaintController::onSizeChange(float newPenSize, int userData)
{
    mCurrPenSize = newPenSize;
    mBrush.onSizeChange(newPenSize);
}

void PaintController::onColorChange(const LGL::Color &color, int userData)
{
    mCurrColor = color;
    mBrush.onColorChange(mCurrColor);
}