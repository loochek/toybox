#include "Slider.hpp"
#include "Button.hpp"
#include "SliderThumb.hpp"
#include "../GUILogic/SliderSideButtonsDelegate.hpp"

const int SIDE_BUTTONS_LENGTH = 20;

const int DEFAULT_MAX_VALUE = 20;

Slider::Slider(const IntRect &widgetRect, Widget *parent) : Widget(widgetRect, parent),
    mValue(0), mMaxValue(DEFAULT_MAX_VALUE)
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

    SliderThumb *thumb = new SliderThumb(IntRect(Vec2i(30, 0), Vec2i(10, 20)), this);
    addChild(thumb);
}

Slider::~Slider()
{
    delete mSideButtonsDelegate;
}

void Slider::onUpdateThis(float elapsedTime)
{
    printf("%d\n", mValue);
}