#include "PaintController.hpp"
#include "../GUIBase/WindowManager.hpp"
#include "../GUIElements/PaintWindow.hpp"
#include "../GUIElements/ColorPickerWindow.hpp"
#include "../GUIElements/ColorPicker.hpp"
#include "../GUIElements/SizePickerWindow.hpp"
#include "../GUIElements/SizePicker.hpp"

const IntRect CANVAS_INIT_RECT      = IntRect(Vec2i(200, 200), Vec2i(700, 400));
const Vec2i   COLOR_PICKER_INIT_POS = Vec2i(1000, 500);
const Vec2i   SIZE_PICKER_INIT_POS  = Vec2i(1000, 100);

PaintController::PaintController(WindowManager *root) : 
    mRoot(root), mColorPicker(nullptr), mSizePicker(nullptr)
{
}

void PaintController::createCanvas()
{
    PaintWindow *canvas = new PaintWindow(CANVAS_INIT_RECT, this, mRoot);
    if (mColorPicker != nullptr)
        mColorPicker->getColorPicker()->subscribeCanvas(canvas->getCanvas());

    if (mSizePicker != nullptr)
        mSizePicker->getSizePicker()->subscribeCanvas(canvas->getCanvas());

    mCanvases.insert(canvas);
    mRoot->addChild(canvas);
}

void PaintController::openColorPicker()
{
    if (mColorPicker != nullptr)
        return;

    mColorPicker = new ColorPickerWindow(COLOR_PICKER_INIT_POS, this, mRoot);
    for (PaintWindow *canvas : mCanvases)
        mColorPicker->getColorPicker()->subscribeCanvas(canvas->getCanvas());

    mRoot->addChild(mColorPicker);
}

void PaintController::openSizePicker()
{
    if (mSizePicker != nullptr)
        return;

    mSizePicker = new SizePickerWindow(SIZE_PICKER_INIT_POS, this, mRoot);
    for (PaintWindow *canvas : mCanvases)
        mSizePicker->getSizePicker()->subscribeCanvas(canvas->getCanvas());

    mRoot->addChild(mSizePicker);
}

void PaintController::onCanvasClose(PaintWindow *canvas)
{
    if (mColorPicker != nullptr)
        mColorPicker->getColorPicker()->unsubscribeCanvas(canvas->getCanvas());

    if (mSizePicker != nullptr)
        mSizePicker->getSizePicker()->unsubscribeCanvas(canvas->getCanvas());

    mCanvases.erase(canvas);
}

void PaintController::onColorPickerClose()
{
    mColorPicker = nullptr;
}

void PaintController::onSizePickerClose()
{
    mSizePicker = nullptr;
}