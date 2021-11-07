#include "PaintController.hpp"
#include "../GUIBase/WindowManager.hpp"
#include "../GUIBase/Canvas.hpp"
#include "../GUIElements/PaintWindow.hpp"
#include "../GUIElements/PalleteWindow.hpp"
#include "../GUIElements/Pallete.hpp"
#include "../GUIElements/SizePickerWindow.hpp"
#include "../GUIElements/SizePicker.hpp"

const IntRect CANVAS_INIT_RECT      = IntRect(Vec2i(200, 200), Vec2i(700, 400));
const Vec2i   COLOR_PICKER_INIT_POS = Vec2i(1000, 300);
const Vec2i   SIZE_PICKER_INIT_POS  = Vec2i(1000, 100);

PaintController::PaintController(WindowManager *root) : 
    mRoot(root), mPallete(nullptr), mSizePicker(nullptr), mCurrPenSize(1.0f)
{
}

void PaintController::createCanvas()
{
    PaintWindow *paintWindow = new PaintWindow(CANVAS_INIT_RECT, this, mRoot);
    paintWindow->getCanvas()->setDrawingColor(mCurrColor);
    paintWindow->getCanvas()->setPenSize(mCurrPenSize);

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

void PaintController::onCanvasClose(PaintWindow *paintWindow)
{
    mPaintWindows.erase(paintWindow);
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

    for (PaintWindow *paintWindow : mPaintWindows)
        paintWindow->getCanvas()->setPenSize(newPenSize);
}

void PaintController::onColorChange(const LGL::Color &color, int userData)
{
    mCurrColor = color;
    
    for (PaintWindow *paintWindow : mPaintWindows)
        paintWindow->getCanvas()->setDrawingColor(mCurrColor);
}