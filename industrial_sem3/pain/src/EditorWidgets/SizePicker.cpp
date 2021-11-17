#include "SizePicker.hpp"
#include "../BaseGUI/Label.hpp"
#include "../BaseGUI/Slider.hpp"
#include "../BaseGUI/TextBox.hpp"
#include "../EditorLogic/SizePicker/SizePickerController.hpp"

const Vec2i SizePicker::PICKER_SIZE = Vec2i(210, 60);

const Vec2i BRUSH_PREVIEW_POS = Vec2i(180, 30);

const Vec2i LABEL_POS = Vec2i(10, 16);

const Vec2i TEXT_BOX_POS  = Vec2i(90, 8);
const int   TEXT_BOX_SIZE = 40;

const Vec2i SLIDER_POS = Vec2i(10, 40);
const int   SLIDER_SIZE = 120;

SizePicker::SizePicker(const Vec2i &position, Widget *parent) :
    Widget(IntRect(position, PICKER_SIZE), parent), mDelegate(nullptr), mCurrBrushSize(INITIAL_SIZE)
{
    mController = new SizePickerController(this);
    
    mLabel = new Label(LABEL_POS, this);
    mLabel->setText("Brush size:");
    addChild(mLabel);

    mTextBox = new TextBox(TEXT_BOX_POS, TEXT_BOX_SIZE, this);
    mTextBox->setDelegate(mController);

    char newText[MAX_TEXT_BOX_LEN + 1] = {0};
    snprintf(newText, MAX_TEXT_BOX_LEN, "%d", INITIAL_SIZE);
    mTextBox->setText(newText);
    
    addChild(mTextBox);

    mSlider = new Slider(SLIDER_POS, SLIDER_SIZE, this);
    mSlider->setDelegate(mController);
    mSlider->setMaxValue(MAX_SIZE - 1);
    mSlider->setValue(INITIAL_SIZE - 1);
    addChild(mSlider);
}

SizePicker::~SizePicker()
{
    delete mController;
}

void SizePicker::onRedrawThis()
{
    mTexture.drawCircle(BRUSH_PREVIEW_POS, mCurrBrushSize, LGL::Color::Black);
}