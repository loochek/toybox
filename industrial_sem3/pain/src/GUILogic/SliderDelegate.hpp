#ifndef SLIDER_DELEGATE_HPP
#define SLIDER_DELEGATE_HPP

class SliderDelegate
{
public:
    virtual ~SliderDelegate() {};

    virtual void onValueChanged(int newValue) = 0;
};

#endif