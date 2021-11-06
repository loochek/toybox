#include "../TextureManager.hpp"
#include "Slider.hpp"
#include "TextureButton.hpp"
#include "SliderThumb.hpp"
#include "../GUILogic/Slider/SliderDelegate.hpp"
#include "../GUILogic/Slider/SliderSideButtonsDelegate.hpp"

const int SLIDER_HEIGHT = 16;
const int SIDE_BUTTONS_LENGTH = 16;
const int THUMB_WIDTH = 10;

const int DEFAULT_MAX_VALUE = 20;

Slider::Slider(const Vec2i &sliderPos, int sliderSize, Widget *parent) :
    Widget(IntRect(sliderPos, Vec2i(sliderSize, SLIDER_HEIGHT)), parent),
    mValue(0), mMaxValue(DEFAULT_MAX_VALUE), mDelegate(nullptr)
{
    getTextures();

    mSideButtonsDelegate = new SliderSideButtonsDelegate(this);

    TextureButton *leftButton = new TextureButton(IntRect(Vec2i(), Vec2i(SIDE_BUTTONS_LENGTH, SLIDER_HEIGHT)),
                                                  mSliderLeftButtonTexture,
                                                  mSliderLeftButtonTexture,
                                                  mSliderLeftButtonTexture,
                                                  this);
    leftButton->setDelegate(mSideButtonsDelegate);
    leftButton->setUserData((int)SliderButton::Decrease);
    addChild(leftButton);

    TextureButton *rightButton = new TextureButton(IntRect(Vec2i(sliderSize - SIDE_BUTTONS_LENGTH, 0),
                                                           Vec2i(SIDE_BUTTONS_LENGTH, SLIDER_HEIGHT)),
                                                   mSliderRightButtonTexture,
                                                   mSliderRightButtonTexture,
                                                   mSliderRightButtonTexture,
                                                   this);
    rightButton->setDelegate(mSideButtonsDelegate);
    rightButton->setUserData((int)SliderButton::Increase);
    addChild(rightButton);

    mThumb = new SliderThumb(IntRect(Vec2i(SIDE_BUTTONS_LENGTH, 0), Vec2i(THUMB_WIDTH, SLIDER_HEIGHT)),
                             SIDE_BUTTONS_LENGTH,
                             sliderSize - SIDE_BUTTONS_LENGTH,
                             this);
    addChild(mThumb);
}

Slider::~Slider()
{
    delete mSideButtonsDelegate;
}

void Slider::thumbMoved(int newThumbXPos)
{
    mValue = (newThumbXPos - mThumb->mLeftLimit) * mMaxValue / (mThumb->mRightLimit - mThumb->mLeftLimit);
    if (mDelegate != nullptr)
        mDelegate->onValueChange(mValue, mUserData);
}

void Slider::setValue(int value)
{
    mValue = std::max(value, 0);
    mValue = std::min(mValue, mMaxValue);

    int newThumbXPos = mThumb->mLeftLimit + mValue * (mThumb->mRightLimit - mThumb->mLeftLimit) / mMaxValue;
    mThumb->setPosition(Vec2i(newThumbXPos, 0));
    
    if (mDelegate != nullptr)
        mDelegate->onValueChange(mValue, mUserData);
}

void Slider::getTextures()
{
    mSliderLeftButtonTexture  = TextureManager::getInstance()->getTexture("slider_button_left");
    mSliderRightButtonTexture = TextureManager::getInstance()->getTexture("slider_button_right");

    if (mSliderLeftButtonTexture  == nullptr ||
        mSliderRightButtonTexture == nullptr)
    {
        throw std::runtime_error("Slider textures are not loaded");
    }
}