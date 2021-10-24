#include "PaintController.hpp"
#include "../GUIElements/PaintWindow.hpp"
#include "../GUIElements/ColorPickerWindow.hpp"
#include "../GUIElements/ColorPicker.hpp"
#include "../GUIElements/SizePickerWindow.hpp"
#include "../GUIElements/SizePicker.hpp"

const IntRect CANVAS_INIT_POS = IntRect(Vec2i(200, 200), Vec2i(700, 400);

PaintController::PaintController(WindowManager *root) : mRoot(root)
{
}

void PaintController::createCanvas()
{
    PaintWindow *canvas = new PaintWindow(CANVAS_INIT_POS, mRoot);
    mCanvases.insert(canvas);

    if (mColorPicker != nullptr)
        mColorPicker->getColorPicker()->subscribeCanvas(canvas->getCanvas());

    if (mSizePicker != nullptr)
        mSizePicker->getSizePicker()->subscribeCanvas(canvas->getCanvas());
}