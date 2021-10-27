#include "Slider.hpp"
#include "Button.hpp"
#include "SliderThumb.hpp"
#include "../GUILogic/SliderDelegate.hpp"
#include "../GUILogic/SliderSideButtonsDelegate.hpp"

const int SIDE_BUTTONS_LENGTH = 20;
const int THUMB_WIDTH = 10;

const int DEFAULT_MAX_VALUE = 20;

Slider::Slider(const IntRect &widgetRect, Widget *parent) : Widget(widgetRect, parent),
    mValue(0), mMaxValue(DEFAULT_MAX_VALUE), mDelegate(nullptr)
{
    mSideButtonsDelegate = new SliderSideButtonsDelegate(this);

    Button *leftButton = new Button(IntRect(Vec2i(), Vec2i(SIDE_BUTTONS_LENGTH, widgetRect.size.y)),
                                    this);
    leftButton->setDelegate(mSideButtonsDelegate);
    leftButton->setUserData((int)SliderButton::Decrease);
    addChild(leftButton);

    Button *rightButton = new Button(IntRect(Vec2i(widgetRect.size.x - SIDE_BUTTONS_LENGTH, 0),
                                             Vec2i(SIDE_BUTTONS_LENGTH, widgetRect.size.y)),
                                     this);
    rightButton->setDelegate(mSideButtonsDelegate);
    rightButton->setUserData((int)SliderButton::Increase);
    addChild(rightButton);

    mThumb = new SliderThumb(IntRect(Vec2i(SIDE_BUTTONS_LENGTH, 0),
                                     Vec2i(THUMB_WIDTH, widgetRect.size.y)),
                             SIDE_BUTTONS_LENGTH,
                             widgetRect.size.x - SIDE_BUTTONS_LENGTH,
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
        mDelegate->onValueChanged(mValue);
}

void Slider::setValue(int value)
{
    mValue = std::max(value, 0);
    mValue = std::min(mValue, mMaxValue);

    int newThumbXPos = mThumb->mLeftLimit + mValue * (mThumb->mRightLimit - mThumb->mLeftLimit) / mMaxValue;
    mThumb->setPosition(Vec2i(newThumbXPos, 0));
    
    if (mDelegate != nullptr)
        mDelegate->onValueChanged(mValue);
}

void Slider::onUpdateThis(float elapsedTime)
{
}