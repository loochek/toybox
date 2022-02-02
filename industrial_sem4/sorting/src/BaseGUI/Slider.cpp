#include "../TextureManager.hpp"
#include "Slider.hpp"
#include "TextureButton.hpp"
#include "SliderThumb.hpp"
#include "../BaseGUILogic/Slider/SliderDelegate.hpp"
#include "../BaseGUILogic/Slider/SliderSideButtonsDelegate.hpp"

const int Slider::SLIDER_HEIGHT = 16;

const int SIDE_BUTTONS_LENGTH = 16;
const int MIN_THUMB_WIDTH     = 18;
const int PIXELS_PER_VALUE    = 4;
const int DEFAULT_MAX_VALUE   = 20;

Slider::Slider(const Vec2i &sliderPos, int sliderSize, Widget *parent) :
    Widget(IntRect(sliderPos, Vec2i(sliderSize, SLIDER_HEIGHT)), parent),
    mValue(0), mMaxValue(DEFAULT_MAX_VALUE), mDelegate(nullptr)
{
    getTextures();

    mSideButtonsDelegate = new SliderSideButtonsDelegate(this);

    TextureButton *leftButton = new TextureButton(IntRect(Vec2i(), Vec2i(SIDE_BUTTONS_LENGTH, SLIDER_HEIGHT)),
                                                  mSliderLeftButtonIdleTexture,
                                                  mSliderLeftButtonHoveredTexture,
                                                  mSliderLeftButtonPressedTexture,
                                                  this);
    leftButton->setDelegate(mSideButtonsDelegate);
    leftButton->setUserData((int)SliderButton::Decrease);
    addChild(leftButton);

    TextureButton *rightButton = new TextureButton(IntRect(Vec2i(sliderSize - SIDE_BUTTONS_LENGTH, 0),
                                                           Vec2i(SIDE_BUTTONS_LENGTH, SLIDER_HEIGHT)),
                                                   mSliderRightButtonIdleTexture,
                                                   mSliderRightButtonHoveredTexture,
                                                   mSliderRightButtonPressedTexture,
                                                   this);
    rightButton->setDelegate(mSideButtonsDelegate);
    rightButton->setUserData((int)SliderButton::Increase);
    addChild(rightButton);

    mThumb = new SliderThumb(Vec2i(SIDE_BUTTONS_LENGTH, 0), calcThumbSize(sliderSize, DEFAULT_MAX_VALUE),
                             SIDE_BUTTONS_LENGTH,
                             sliderSize - SIDE_BUTTONS_LENGTH,
                             this);
    addChild(mThumb);
}

Slider::~Slider()
{
    delete mSideButtonsDelegate;
}

void Slider::setValue(int value)
{
    mValue = std::max(value, 0);
    mValue = std::min(mValue, mMaxValue);

    int newThumbXPos = mThumb->mLeftLimit + mValue * (mThumb->mRightLimit - mThumb->mLeftLimit) / mMaxValue;
    mThumb->setPosition(Vec2i(newThumbXPos, 0));
}

void Slider::setMaxValue(int value)
{
    mMaxValue = value;

    mThumb->resize(calcThumbSize(mRect.size.x, mMaxValue));

    int newThumbXPos = mThumb->mLeftLimit + mValue * (mThumb->mRightLimit - mThumb->mLeftLimit) / mMaxValue;
    mThumb->setPosition(Vec2i(newThumbXPos, 0));
};

void Slider::onRedrawThis()
{
    mTexture.drawTexture(*mSliderBodyTexture, Vec2i(), IntRect(Vec2i(), mRect.size));
}

void Slider::thumbMoved(int newThumbXPos)
{
    mValue = (newThumbXPos - mThumb->mLeftLimit) * mMaxValue / (mThumb->mRightLimit - mThumb->mLeftLimit);
    if (mDelegate != nullptr)
        mDelegate->onValueChange(mValue, mUserData);
}

void Slider::getTextures()
{
    mSliderLeftButtonIdleTexture     = TextureManager::getInstance()->getTexture("slider_button_left_idle");
    mSliderLeftButtonHoveredTexture  = TextureManager::getInstance()->getTexture("slider_button_left_hovered");
    mSliderLeftButtonPressedTexture  = TextureManager::getInstance()->getTexture("slider_button_left_pressed");
    mSliderRightButtonIdleTexture    = TextureManager::getInstance()->getTexture("slider_button_right_idle");
    mSliderRightButtonHoveredTexture = TextureManager::getInstance()->getTexture("slider_button_right_hovered");
    mSliderRightButtonPressedTexture = TextureManager::getInstance()->getTexture("slider_button_right_pressed");
    mSliderBodyTexture               = TextureManager::getInstance()->getTexture("slider_body");

    if (mSliderLeftButtonIdleTexture == nullptr ||
        mSliderLeftButtonHoveredTexture == nullptr ||
        mSliderLeftButtonPressedTexture == nullptr ||
        mSliderRightButtonIdleTexture == nullptr ||
        mSliderRightButtonHoveredTexture == nullptr ||
        mSliderRightButtonPressedTexture == nullptr ||
        mSliderBodyTexture == nullptr)
    {
        throw std::runtime_error("Slider textures are not loaded");
    }
}

int Slider::calcThumbSize(int sliderSize, int maxValue)
{
    sliderSize -= 2 * SIDE_BUTTONS_LENGTH;

    int workingSize = maxValue * PIXELS_PER_VALUE;
    return std::max(sliderSize - workingSize, MIN_THUMB_WIDTH);
}